"""
Created on 9 Jan 2014

@author: sjw
"""
import os
import collections
import datetime
import operator
import math
import numpy as np

import pipeline.domain.measures as measures
import pipeline.infrastructure.utils as utils
import pipeline.infrastructure.casatools as casatools
import pipeline.infrastructure.logging as logging
import pipeline.infrastructure.pipelineqa as pqa
import pipeline.infrastructure.renderer.rendererutils as rutils

import pipeline.qa.checksource as checksource

__all__ = ['score_polintents',                                # ALMA specific
           'score_bands',                                     # ALMA specific
           'score_bwswitching',                               # ALMA specific
           'score_tsysspwmap',                                # ALMA specific
           'score_number_antenna_offsets',                    # ALMA specific
           '_score_missing_derived_fluxes',                   # ALMA specific
           'score_derived_fluxes_snr',                        # ALMA specific
           'score_phaseup_mapping_fraction',                  # ALMA specific
           'score_refspw_mapping_fraction',                   # ALMA specific
           'score_missing_phaseup_snrs',                      # ALMA specific
           'score_missing_bandpass_snrs',                     # ALMA specific
           'score_poor_phaseup_solutions',                    # ALMA specific
           'score_poor_bandpass_solutions',                   # ALMA specific
           'score_missing_phase_snrs',                        # ALMA specific
           'score_poor_phase_snrs',                           # ALMA specific
           'score_flagging_view_exists',                      # ALMA specific
           'score_checksources',                              # ALMA specific
           'score_file_exists',
           'score_path_exists',
           'score_flags_exist',
           'score_applycmds_exist',
           'score_caltables_exist',
           'score_setjy_measurements',
           'score_missing_intents',
           'score_ephemeris_coordinates',
           'score_online_shadow_template_agents',
           'score_applycal_agents',
           'score_total_data_flagged',
           'score_ms_model_data_column_present',
           'score_ms_history_entries_present',
           'score_contiguous_session']

LOG = logging.get_logger(__name__)


# - utility functions --------------------------------------------------------------------------------------------------

def log_qa(method):
    """
    Decorator that logs QA evaluations as they return with a log level of
    INFO for scores between perfect and 'slightly suboptimal' scores and
    WARNING for any other level.
    """
    def f(self, *args, **kw):
        # get the size of the CASA log before task execution
        qascore = method(self, *args, **kw)
        if qascore.score >= rutils.SCORE_THRESHOLD_SUBOPTIMAL:
            LOG.info(qascore.longmsg)
        else:
            LOG.warning(qascore.longmsg)
        return qascore

    return f

# struct to hold flagging statistics
AgentStats = collections.namedtuple("AgentStats", "name flagged total")


def calc_flags_per_agent(summaries):
    stats = []
    for idx in range(0, len(summaries)):
        flagcount = int(summaries[idx]['flagged'])
        totalcount = int(summaries[idx]['total'])

        # From the second summary onwards, subtract counts from the previous 
        # one
        if idx > 0:
            flagcount -= int(summaries[idx - 1]['flagged'])
        
        stat = AgentStats(name=summaries[idx]['name'],
                          flagged=flagcount,
                          total=totalcount)
        stats.append(stat)

    return stats


def linear_score(x, x1, x2, y1=0.0, y2=1.0):
    """
    Calculate the score for the given data value, assuming the
    score follows a linear gradient between the low and high values.
    
    x values will be clipped to lie within the range x1->x2
    """
    x1 = float(x1)
    x2 = float(x2)
    y1 = float(y1)
    y2 = float(y2)
    
    clipped_x = sorted([x1, x, x2])[1]
    m = (y2-y1) / (x2-x1)
    c = y1 - m*x1
    return m*clipped_x + c


def score_data_flagged_by_agents(ms, summaries, min_frac, max_frac, 
                                 agents=None):
    """
    Calculate a score for the agentflagger summaries based on the fraction of
    data flagged by certain flagging agents.

    min_frac < flagged < max_frac maps to score of 1-0
    """
    agent_stats = calc_flags_per_agent(summaries)

    if agents is None:
        agents = []
    match_all_agents = True if len(agents) is 0 else False

    # sum the number of flagged rows for the selected agents     
    frac_flagged = reduce(operator.add, 
                          [float(s.flagged)/s.total for s in agent_stats
                           if s.name in agents or match_all_agents], 0)

    score = linear_score(frac_flagged, min_frac, max_frac, 1.0, 0.0)
    percent = 100.0 * frac_flagged
    longmsg = ('%0.2f%% data in %s flagged by %s flagging agents'
               '' % (percent, ms.basename, utils.commafy(agents, False)))
    shortmsg = '%0.2f%% data flagged' % percent

    origin = pqa.QAOrigin(metric_name='score_data_flagged_by_agents',
                          metric_score=frac_flagged,
                          metric_units='Fraction of data newly flagged')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


# - exported scoring functions -----------------------------------------------------------------------------------------

def score_ms_model_data_column_present(all_mses, mses_with_column):
    """
    Give a score for a group of mses based on the number with modeldata 
    columns present.
    None with modeldata - 100% with modeldata = 1.0 -> 0.5
    """
    num_with = len(mses_with_column)
    num_all = len(all_mses)
    f = float(num_with) / num_all

    if mses_with_column:
        # log a message like 'No model columns found in a.ms, b.ms or c.ms'
        basenames = [ms.basename for ms in mses_with_column]
        s = utils.commafy(basenames, quotes=False)
        longmsg = 'Model data column found in %s' % s
        shortmsg = '%s/%s have MODELDATA' % (num_with, num_all) 
    else:
        # log a message like 'Model data column was found in a.ms and b.ms'
        basenames = [ms.basename for ms in all_mses]
        s = utils.commafy(basenames, quotes=False, conjunction='or')
        longmsg = ('No model data column found in %s' % s)            
        shortmsg = 'MODELDATA empty' 

    score = linear_score(f, 0.0, 1.0, 1.0, 0.5)

    origin = pqa.QAOrigin(metric_name='score_ms_model_data_column_present',
                          metric_score=f,
                          metric_units='Fraction of MSes with modeldata columns present')

    return pqa.QAScore(score, longmsg, shortmsg, origin=origin)


@log_qa
def score_ms_history_entries_present(all_mses, mses_with_history):
    """
    Give a score for a group of mses based on the number with history 
    entries present.
    None with history - 100% with history = 1.0 -> 0.5
    """
    num_with = len(mses_with_history)
    num_all = len(all_mses)

    if mses_with_history:
        # log a message like 'Entries were found in the HISTORY table for 
        # a.ms and b.ms'
        basenames = utils.commafy([ms.basename for ms in mses_with_history], quotes=False)
        if len(mses_with_history) is 1:
            longmsg = ('Unexpected entries were found in the HISTORY table of %s. '
                       'This measurement set may already be processed.' % basenames)
        else:
            longmsg = ('Unexpected entries were found in the HISTORY tables of %s. '
                       'These measurement sets may already be processed.' % basenames)
        shortmsg = '%s/%s have HISTORY' % (num_with, num_all) 

    else:
        # log a message like 'No history entries were found in a.ms or b.ms'
        basenames = [ms.basename for ms in all_mses]
        s = utils.commafy(basenames, quotes=False, conjunction='or')
        longmsg = 'No HISTORY entries found in %s' % s
        shortmsg = 'No HISTORY entries'

    f = float(num_with) / num_all
    score = linear_score(f, 0.0, 1.0, 1.0, 0.5)

    origin = pqa.QAOrigin(metric_name='score_ms_history_entries_present',
                          metric_score=f,
                          metric_units='Fraction of MSes with HISTORY')

    return pqa.QAScore(score, longmsg, shortmsg, origin=origin)


@log_qa
def score_bwswitching(mses):
    """
    Score a MeasurementSet object based on the presence of 
    bandwidth switching observings. For bandwidth switched
    observations the TARGET and PHASE spws are different.
    """
    score = 1.0
    num_mses = len(mses)
    all_ok = True
    complaints = []
    nophasecals = []

    # analyse each MS
    for ms in mses:
        # Get the science spws
        scispws = set([spw.id for spw in ms.get_spectral_windows(science_windows_only=True)])

        # Get phase calibrator science spw ids
        phasespws = []
        for scan in ms.get_scans(scan_intent='PHASE'):
            phasespws.extend([spw.id for spw in scan.spws])
        phasespws = set(phasespws).intersection(scispws)

        # Get science target science spw ids
        targetspws = []
        for scan in ms.get_scans(scan_intent='TARGET'):
            targetspws.extend([spw.id for spw in scan.spws])
        targetspws = set(targetspws).intersection(scispws)

        # Determine the difference between the two
        nophasecals = targetspws.difference(phasespws)
        if len(nophasecals) == 0:
            continue

        # Score the difference
        all_ok = False
        for _ in nophasecals:
            score += (-1.0 / num_mses / len(nophasecals))
        longmsg = ('%s contains no phase calibrations for target spws %s'
                   '' % (ms.basename, utils.commafy(nophasecals, False)))
        complaints.append(longmsg)

    if all_ok:
        longmsg = ('Phase calibrations found for all target spws in %s.' % (
                   utils.commafy([ms.basename for ms in mses], False)))
        shortmsg = 'Phase calibrations found for all target spws' 
    else:
        longmsg = '%s.' % utils.commafy(complaints, False)
        shortmsg = 'No phase calibrations found for target spws %s' % list(nophasecals)

    origin = pqa.QAOrigin(metric_name='score_bwswitching',
                          metric_score=len(nophasecals),
                          metric_units='Number of MSes without phase calibrators')

    return pqa.QAScore(max(0.0, score), longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_bands(mses):
    """
    Score a MeasurementSet object based on the presence of 
    ALMA bands with calibration issues.
    """

    # ALMA receiver bands. Warnings will be raised for any 
    # measurement sets containing the following bands.
    score = 1.0
    score_map = {'8': -1.0,
                 '9': -1.0}

    unsupported = set(score_map.keys())

    num_mses = len(mses)
    all_ok = True
    complaints = []

    # analyse each MS
    for ms in mses:
        msbands = []
        for spw in ms.get_spectral_windows(science_windows_only=True):
            bandnum = spw.band.split(' ')[2]
            msbands.append(bandnum)
        msbands = set(msbands)
        overlap = unsupported.intersection(msbands)
        if not overlap:
            continue
        all_ok = False
        for m in overlap:
            score += (score_map[m] / num_mses)
        longmsg = ('%s contains band %s data'
                   '' % (ms.basename, utils.commafy(overlap, False)))
        complaints.append(longmsg)

    if all_ok:
        longmsg = ('No high frequency %s band data were found in %s.' % (list(unsupported),
                   utils.commafy([ms.basename for ms in mses], False)))
        shortmsg = 'No high frequency band data found' 
    else:
        longmsg = '%s.' % utils.commafy(complaints, False)
        shortmsg = 'High frequency band data found'

    origin = pqa.QAOrigin(metric_name='score_bands',
                          metric_score=score,
                          metric_units='MS score based on presence of high-frequency data')

    # Make score linear
    return pqa.QAScore(max(0.0, score), longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_polintents(mses):
    """
    Score a MeasurementSet object based on the presence of 
    polarization intents.
    """

    # Polarization intents. Warnings will be raised for any 
    # measurement sets containing these intents. Ignore the
    # array type for now.
    score = 1.0
    score_map = {
        'POLARIZATION': -1.0,
        'POLANGLE': -1.0,
        'POLLEAKAGE': -1.0
    }

    unsupported = set(score_map.keys())

    num_mses = len(mses)
    all_ok = True
    complaints = []

    # analyse each MS
    for ms in mses:
        # are these intents present in the ms
        overlap = unsupported.intersection(ms.intents)
        if not overlap:
            continue
        all_ok = False
        for m in overlap:
            score += (score_map[m] / num_mses)

        longmsg = ('%s contains %s polarization calibration intents'
                   '' % (ms.basename, utils.commafy(overlap, False)))
        complaints.append(longmsg)

    if all_ok:
        longmsg = ('No polarization calibration intents were found in '
                   '%s.' % utils.commafy([ms.basename for ms in mses], False))
        shortmsg = 'No polarization calibrators found'
    else:
        longmsg = '%s.' % utils.commafy(complaints, False)
        shortmsg = 'Polarization calibrators found'

    origin = pqa.QAOrigin(metric_name='score_polintents',
                          metric_score=score,
                          metric_units='MS score based on presence of polarisation data')

    return pqa.QAScore(max(0.0, score), longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_missing_intents(mses, array_type='ALMA_12m'):
    """
    Score a MeasurementSet object based on the presence of certain
    observing intents.
    """
    # Required calibration intents. Warnings will be raised for any 
    # measurement sets missing these intents 
    score = 1.0
    if array_type == 'ALMA_TP':
        score_map = {'ATMOSPHERE': -1.0}
    else:
        score_map = {
            'PHASE': -1.0,
            'BANDPASS': -0.1,
            'AMPLITUDE': -0.1
        }

    required = set(score_map.keys())

    num_mses = len(mses)
    all_ok = True
    complaints = []

    # analyse each MS
    for ms in mses:
        # do we have the necessary calibrators?
        if not required.issubset(ms.intents):
            all_ok = False
            missing = required.difference(ms.intents)
            for m in missing:
                score += (score_map[m] / num_mses)

            longmsg = ('%s is missing %s calibration intents'
                       '' % (ms.basename, utils.commafy(missing, False)))
            complaints.append(longmsg)
            
    if all_ok:
        longmsg = ('All required calibration intents were found in '
                   '%s.' % utils.commafy([ms.basename for ms in mses], False))
        shortmsg = 'All calibrators found'
    else:
        longmsg = '%s.' % utils.commafy(complaints, False)
        shortmsg = 'Calibrators missing'

    origin = pqa.QAOrigin(metric_name='score_missing_intents',
                          metric_score=score,
                          metric_units='Score based on missing calibration intents')

    return pqa.QAScore(max(0.0, score), longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_ephemeris_coordinates(mses):

    """
    Score a MeasurementSet object based on the presence of possible
    ephemeris coordinates.
    """

    score = 1.0

    num_mses = len(mses)
    all_ok = True
    complaints = []
    zero_direction = casatools.measures.direction('j2000', '0.0deg', '0.0deg')
    zero_ra = casatools.quanta.formxxx(zero_direction['m0'], format='hms', prec=3)
    zero_dec = casatools.quanta.formxxx(zero_direction['m1'], format='dms', prec=2)

    # analyse each MS
    for ms in mses:
        # Examine each source
        for source in ms.sources:
            if source.ra == zero_ra and source.dec == zero_dec:
                all_ok = False
                score += (-1.0 / num_mses)
                longmsg = ('Suspicious source coordinates for  %s in %s. Check whether position of '
                           '00:00:00.0+00:00:00.0 is valid.' % (source.name, ms.basename))
                complaints.append(longmsg)

    if all_ok:
        longmsg = ('All source coordinates OK in '
                   '%s.' % utils.commafy([ms.basename for ms in mses], False))
        shortmsg = 'All source coordinates OK'
    else:
        longmsg = '%s.' % utils.commafy(complaints, False)
        shortmsg = 'Suspicious source coordinates'

    origin = pqa.QAOrigin(metric_name='score_ephemeris_coordinates',
                          metric_score=score,
                          metric_units='Score based on presence of ephemeris coordinates')

    return pqa.QAScore(max(0.0, score), longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_online_shadow_template_agents(ms, summaries):
    """
    Get a score for the fraction of data flagged by online, shadow, and template agents.

    0 < score < 1 === 60% < frac_flagged < 5%
    """
    score = score_data_flagged_by_agents(ms, summaries, 0.05, 0.6,
                                         ['online', 'shadow', 'qa0', 'before', 'template'])

    new_origin = pqa.QAOrigin(metric_name='score_online_shadow_template_agents',
                              metric_score=score.origin.metric_score,
                              metric_units='Fraction of data newly flagged by online, shadow, and template agents')
    score.origin = new_origin

    return score


@log_qa
def score_applycal_agents(ms, summaries):
    """
    Get a score for the fraction of data flagged by applycal agents.

    0 < score < 1 === 60% < frac_flagged < 5%
    """
    score = score_data_flagged_by_agents(ms, summaries, 0.05, 0.6, ['applycal'])

    new_origin = pqa.QAOrigin(metric_name='score_applycal_agents',
                              metric_score=score.origin.metric_score,
                              metric_units=score.origin.metric_units)
    score.origin = new_origin

    return score


@log_qa
def score_flagging_view_exists(filename, result):
    """
    Assign a score of zero if the flagging view cannot be computed
    """

    # By default, assume no flagging views were found.
    score = 0.0
    longmsg = 'No flagging views for %s' % filename
    shortmsg = 'No flagging views'

    # Check if this is a flagging result for a single metric, where
    # the flagging view is stored directly in the result.
    try:
        view = result.view
        if view:
            score = 1.0
            longmsg = 'Flagging views exist for %s' % filename
            shortmsg = 'Flagging views exist'
    except AttributeError:
        pass
    
    # Check if this flagging results contains multiple metrics,
    # and look for flagging views among components.
    try:
        # Set score to 1 as soon as a single metric contains a
        # valid flagging view.
        for metricresult in result.components.values():
            view = metricresult.view
            if view:
                score = 1.0
                longmsg = 'Flagging views exist for %s' % filename
                shortmsg = 'Flagging views exist'
    except AttributeError:
        pass

    origin = pqa.QAOrigin(metric_name='score_flagging_view_exists',
                          metric_score=bool(score),
                          metric_units='Presence of flagging view')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=filename, origin=origin)


@log_qa
def score_total_data_flagged(filename, summaries):
    """
    Calculate a score for the flagging task based on the total fraction of
    data flagged.
    
    0%-5% flagged   -> 1
    5%-50% flagged  -> 0.5
    50-100% flagged -> 0
    """    
    agent_stats = calc_flags_per_agent(summaries)

    # sum the number of flagged rows for the selected agents     
    frac_flagged = reduce(operator.add, 
                          [float(s.flagged)/s.total for s in agent_stats], 0)

    if frac_flagged > 0.5:
        score = 0
    else:
        score = linear_score(frac_flagged, 0.05, 0.5, 1.0, 0.5)

    percent = 100.0 * frac_flagged
    longmsg = '%0.2f%% of data in %s was flagged' % (percent, filename)
    shortmsg = '%0.2f%% data flagged' % percent

    origin = pqa.QAOrigin(metric_name='score_total_data_flagged',
                          metric_score=frac_flagged,
                          metric_units='Total fraction of data that is flagged')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=os.path.basename(filename), origin=origin)


@log_qa
def score_fraction_newly_flagged(filename, summaries, vis):
    """
    Calculate a score for the flagging task based on the fraction of
    data newly flagged.
    
    0%-5% flagged   -> 1
    5%-50% flagged  -> 0.5
    50-100% flagged -> 0
    """    
    agent_stats = calc_flags_per_agent(summaries)

    # sum the number of flagged rows for the selected agents     
    frac_flagged = reduce(operator.add, 
                          [float(s.flagged)/s.total for s in agent_stats[1:]], 0)
        
    if frac_flagged > 0.5:
        score = 0
    else:
        score = linear_score(frac_flagged, 0.05, 0.5, 1.0, 0.5)

    percent = 100.0 * frac_flagged
    longmsg = '%0.2f%% of data in %s was newly flagged' % (percent, filename)
    shortmsg = '%0.2f%% data flagged' % percent

    origin = pqa.QAOrigin(metric_name='score_fraction_newly_flagged',
                          metric_score=frac_flagged,
                          metric_units='Fraction of data that is newly flagged')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=os.path.basename(vis), origin=origin)


@log_qa
def linear_score_fraction_newly_flagged(filename, summaries, vis):
    """
    Calculate a score for the flagging task based on the fraction of
    data newly flagged.
    
    fraction flagged   -> score
    """    
    agent_stats = calc_flags_per_agent(summaries)

    # sum the number of flagged rows for the selected agents     
    frac_flagged = reduce(operator.add, 
                          [float(s.flagged)/s.total for s in agent_stats[1:]], 0)

    score = 1.0 - frac_flagged        

    percent = 100.0 * frac_flagged
    longmsg = '%0.2f%% of data in %s was newly flagged' % (percent, filename)
    shortmsg = '%0.2f%% data flagged' % percent

    origin = pqa.QAOrigin(metric_name='linear_score_fraction_newly_flagged',
                          metric_score=frac_flagged,
                          metric_units='Fraction of data that is newly flagged')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=os.path.basename(vis), origin=origin)


@log_qa
def score_contiguous_session(mses, tolerance=datetime.timedelta(hours=1)):
    """
    Check whether measurement sets are contiguous in time. 
    """
    # only need to check when given multiple measurement sets
    if len(mses) < 2:
        origin = pqa.QAOrigin(metric_name='score_contiguous_session',
                              metric_score=0,
                              metric_units='Non-contiguous measurement sets present')
        return pqa.QAScore(1.0,
                           longmsg='%s forms one continuous observing session.' % mses[0].basename,
                           shortmsg='Unbroken observing session',
                           vis=mses[0].basename,
                           origin=origin)

    # reorder MSes by start time
    by_start = sorted(mses, 
                      key=lambda m: utils.get_epoch_as_datetime(m.start_time))

    # create an interval for each one, including our tolerance    
    intervals = []    
    for ms in by_start:
        start = utils.get_epoch_as_datetime(ms.start_time)
        end = utils.get_epoch_as_datetime(ms.end_time)
        interval = measures.TimeInterval(start - tolerance, end + tolerance)
        intervals.append(interval)

    # check whether the intervals overlap
    bad_mses = []
    for i, (interval1, interval2) in enumerate(zip(intervals[0:-1], 
                                                   intervals[1:])):
        if not interval1.overlaps(interval2):
            bad_mses.append(utils.commafy([by_start[i].basename,
                                           by_start[i+1].basename]))

    if bad_mses:
        basenames = utils.commafy(bad_mses, False)
        longmsg = ('Measurement sets %s are not contiguous. They may be '
                   'miscalibrated as a result.' % basenames)
        shortmsg = 'Gaps between observations'
        score = 0.5
    else:
        basenames = utils.commafy([ms.basename for ms in mses])
        longmsg = ('Measurement sets %s are contiguous.' % basenames)
        shortmsg = 'Unbroken observing session'
        score = 1.0

    origin = pqa.QAOrigin(metric_name='score_contiguous_session',
                          metric_score=not bool(bad_mses),
                          metric_units='Non-contiguous measurement sets present')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_wvrgcal(ms_name, wvr_score):
    if wvr_score < 1.0:
        score = 0
    else:
        score = linear_score(wvr_score, 1.0, 2.0, 0.5, 1.0)

    longmsg = 'RMS improvement was %0.2f for %s' % (wvr_score, ms_name)
    shortmsg = '%0.2fx improvement' % wvr_score

    origin = pqa.QAOrigin(metric_name='score_wvrgcal',
                          metric_score=wvr_score,
                          metric_units='Phase RMS improvement after applying WVR correction')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=os.path.basename(ms_name),
        origin=origin)


@log_qa
def score_sdtotal_data_flagged(label, frac_flagged):
    """
    Calculate a score for the flagging task based on the total fraction of
    data flagged.
    
    0%-5% flagged   -> 1
    5%-50% flagged  -> 0.5
    50-100% flagged -> 0
    """
    if frac_flagged > 0.5:
        score = 0
    else:
        score = linear_score(frac_flagged, 0.05, 0.5, 1.0, 0.5)
    
    percent = 100.0 * frac_flagged
    longmsg = '%0.2f%% of data in %s was newly flagged' % (percent, label)
    shortmsg = '%0.2f%% data flagged' % percent

    origin = pqa.QAOrigin(metric_name='score_sdtotal_data_flagged',
                          metric_score=frac_flagged,
                          metric_units='Fraction of data newly flagged')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=None, origin=origin)


@log_qa
def score_sdtotal_data_flagged_old(name, ant, spw, pol, frac_flagged, field=None):
    """
    Calculate a score for the flagging task based on the total fraction of
    data flagged.
    
    0%-5% flagged   -> 1
    5%-50% flagged  -> 0.5
    50-100% flagged -> 0
    """
    if frac_flagged > 0.5:
        score = 0
    else:
        score = linear_score(frac_flagged, 0.05, 0.5, 1.0, 0.5)
    
    percent = 100.0 * frac_flagged
    if field is None:
        longmsg = '%0.2f%% of data in %s (Ant=%s, SPW=%d, Pol=%d) was flagged' % (percent, name, ant, spw, pol)
    else:
        longmsg = ('%0.2f%% of data in %s (Ant=%s, Field=%s, SPW=%d, Pol=%s) was '
                   'flagged' % (percent, name, ant, field, spw, pol))
    shortmsg = '%0.2f%% data flagged' % percent

    origin = pqa.QAOrigin(metric_name='score_sdtotal_data_flagged_old',
                          metric_score=frac_flagged,
                          metric_units='Fraction of data newly flagged')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=os.path.basename(name), origin=origin)


@log_qa
def score_tsysspwmap(ms, unmappedspws):
    """
    Score is equal to the fraction of unmapped windows
    """

    if len(unmappedspws) <= 0:
        score = 1.0
        longmsg = 'Tsys spw map is complete for %s ' % ms.basename
        shortmsg = 'Tsys spw map is complete'
    else:
        nscispws = len([spw.id for spw in ms.get_spectral_windows(science_windows_only=True)])
        if nscispws <= 0:
            score = 0.0
        else:
            score = float(nscispws - len(unmappedspws)) / float(nscispws)
        longmsg = 'Tsys spw map is incomplete for %s science window%s ' % (ms.basename,
                                                                           utils.commafy(unmappedspws, False, 's'))
        shortmsg = 'Tsys spw map is incomplete'

    origin = pqa.QAOrigin(metric_name='score_tsysspwmap',
                          metric_score=score,
                          metric_units='Fraction of unmapped Tsys windows')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_setjy_measurements(ms, reqfields, reqintents, reqspws, measurements):
    """
    Score is equal to the ratio of the number of actual flux
    measurements to expected number of flux measurements
    """

    # Expected fields
    scifields = {field for field in ms.get_fields(reqfields, intent=reqintents)}

    # Expected science windows
    scispws = {spw.id for spw in ms.get_spectral_windows(reqspws, science_windows_only=True)}

    # Loop over the expected fields
    nexpected = 0
    for scifield in scifields:
        validspws = set([spw.id for spw in scifield.valid_spws])
        nexpected += len(validspws.intersection(scispws))

    # Loop over the measurements
    nmeasured = 0
    for value in measurements.itervalues():
        # Loop over the flux measurements
        nmeasured += len(value)

    # Compute score
    if nexpected == 0:
        score = 0.0
        longmsg = 'No flux calibrators for %s ' % ms.basename
        shortmsg = 'No flux calibrators'
    elif nmeasured == 0:
        score = 0.0
        longmsg = 'No flux measurements for %s ' % ms.basename
        shortmsg = 'No flux measurements'
    elif nexpected == nmeasured:
        score = 1.0
        longmsg = 'All expected flux calibrator measurements present for %s ' % ms.basename
        shortmsg = 'All expected flux calibrator measurements present'
    elif nmeasured < nexpected:
        score = float(nmeasured) / float(nexpected)
        longmsg = 'Missing flux calibrator measurements for %s %d/%d ' % (ms.basename, nmeasured, nexpected)
        shortmsg = 'Missing flux calibrator measurements'
    else:
        score = 0.0
        longmsg = 'Too many flux calibrator measurements for %s %d/%d' % (ms.basename, nmeasured, nexpected)
        shortmsg = 'Too many flux measurements'

    origin = pqa.QAOrigin(metric_name='score_setjy_measurements',
                          metric_score=score,
                          metric_units='Ratio of number of flux measurements to number expected')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_number_antenna_offsets(ms, antenna, offsets):
    """
    Score is 1.0 if no antenna needed a position offset correction, and
    set to the "suboptimal" threshold if at least one antenna needed a 
    correction.
    """
    nant_with_offsets = len(offsets) / 3

    if nant_with_offsets == 0:
        score = 1.0
        longmsg = 'No antenna position offsets for %s ' % ms.basename
        shortmsg = 'No antenna position offsets'
    else:
        # CAS-8877: if at least 1 antenna needed correction, then set the score
        # to the "suboptimal" threshold. 
        score = rutils.SCORE_THRESHOLD_SUBOPTIMAL
        longmsg = '%d nonzero antenna position offsets for %s ' % (nant_with_offsets, ms.basename)
        shortmsg = 'Nonzero antenna position offsets'

    origin = pqa.QAOrigin(metric_name='score_number_antenna_offsets',
                          metric_score=nant_with_offsets,
                          metric_units='Number of antennas requiring position offset correction')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_missing_derived_fluxes(ms, reqfields, reqintents, measurements):
    """
    Score is equal to the ratio of actual flux
    measurement to expected flux measurements
    """
    # Expected fields
    scifields = {field for field in ms.get_fields(reqfields, intent=reqintents)}

    # Expected science windows
    scispws = {spw.id for spw in ms.get_spectral_windows(science_windows_only=True)}

    # Loop over the expected fields
    nexpected = 0
    for scifield in scifields:
        validspws = {spw.id for spw in scifield.valid_spws}
        nexpected += len(validspws.intersection(scispws))

    # Loop over measurements
    nmeasured = 0
    for key, value in measurements.iteritems():
        # Loop over the flux measurements
        for flux in value:
            fluxjy = getattr(flux, 'I').to_units(measures.FluxDensityUnits.JANSKY)
            uncjy = getattr(flux.uncertainty, 'I').to_units(measures.FluxDensityUnits.JANSKY)
            if fluxjy <= 0.0 or uncjy <= 0.0: 
                continue
            nmeasured += 1

    # Compute score
    if nexpected == 0:
        score = 0.0
        longmsg = 'No secondary calibrators for %s ' % ms.basename
        shortmsg = 'No secondary calibrators'
    elif nmeasured == 0:
        score = 0.0
        longmsg = 'No derived fluxes for %s ' % ms.basename
        shortmsg = 'No derived fluxes'
    elif nexpected == nmeasured:
        score = 1.0
        longmsg = 'All expected derived fluxes present for %s ' % ms.basename
        shortmsg = 'All expected derived fluxes present'
    elif nmeasured < nexpected:
        score = float(nmeasured) / float(nexpected)
        longmsg = 'Missing derived fluxes for %s %d/%d' % (ms.basename, nmeasured, nexpected)
        shortmsg = 'Missing derived fluxes'
    else:
        score = 0.0
        longmsg = 'Extra derived fluxes for %s %d/%d' % (ms.basename, nmeasured, nexpected)
        shortmsg = 'Extra derived fluxes'

    origin = pqa.QAOrigin(metric_name='score_missing_derived_fluxes',
                          metric_score=score,
                          metric_units='Ratio of number of flux measurements to number expected')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_refspw_mapping_fraction(ms, ref_spwmap):
    """
    Compute the fraction of science spws that have not been
    mapped to other windows.
    """
    if ref_spwmap == [-1]:
        score = 1.0
        longmsg = 'No mapped science spws for %s ' % ms.basename
        shortmsg = 'No mapped science spws'

        origin = pqa.QAOrigin(metric_name='score_refspw_mapping_fraction',
                              metric_score=0,
                              metric_units='Number of unmapped science spws')
    else:
        # Expected science windows
        scispws = set([spw.id for spw in ms.get_spectral_windows(science_windows_only=True)])
        nexpected = len(scispws)

        nunmapped = 0
        for spwid in scispws:
            if spwid == ref_spwmap[spwid]: 
                nunmapped += 1
        
        if nunmapped >= nexpected:
            score = 1.0
            longmsg = 'No mapped science spws for %s ' % ms.basename
            shortmsg = 'No mapped science spws'
        else:
            # Replace the previous score with a warning
            score = rutils.SCORE_THRESHOLD_WARNING
            longmsg = 'There are %d mapped science spws for %s ' % (nexpected - nunmapped, ms.basename)
            shortmsg = 'There are mapped science spws'

        origin = pqa.QAOrigin(metric_name='score_refspw_mapping_fraction',
                              metric_score=nunmapped,
                              metric_units='Number of unmapped science spws')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_phaseup_mapping_fraction(ms, reqfields, reqintents, phaseup_spwmap):
    """
    Compute the fraction of science spws that have not been
    mapped to other probably wider windows.

    Note that reqfields and reqintents are no longer used. Remove at some point
    """
    nunmapped = 0
    if not phaseup_spwmap:
        score = 1.0
        longmsg = 'No mapped science spws for %s ' % ms.basename
        shortmsg = 'No mapped science spws'
    else:
        # Expected science windows
        scispws = set([spw.id for spw in ms.get_spectral_windows(science_windows_only=True)])
        nexpected = len(scispws)

        for spwid in scispws:
            if spwid == phaseup_spwmap[spwid]:
                nunmapped += 1
        
        if nunmapped >= nexpected:
            score = 1.0
            longmsg = 'No mapped science spws for %s ' % ms.basename
            shortmsg = 'No mapped science spws'
        else:
            # Replace the previous score with a warning
            score = rutils.SCORE_THRESHOLD_WARNING
            longmsg = 'There are %d mapped science spws for %s ' % (nexpected - nunmapped, ms.basename)
            shortmsg = 'There are mapped science spws'

    origin = pqa.QAOrigin(metric_name='score_phaseup_mapping_fraction',
                          metric_score=nunmapped,
                          metric_units='Number of unmapped science spws')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_missing_phaseup_snrs(ms, spwids, phsolints):
    """
    Score is the fraction of spws with phaseup SNR estimates
    """
    # Compute the number of expected and missing SNR measurements
    nexpected = len(spwids)
    missing_spws = []
    for i in range(len(spwids)):
        if not phsolints[i]:
            missing_spws.append(spwids[i])
    nmissing = len(missing_spws) 

    if nexpected <= 0:
        score = 0.0
        longmsg = 'No phaseup SNR estimates for %s ' % ms.basename
        shortmsg = 'No phaseup SNR estimates'
    elif nmissing <= 0:
        score = 1.0
        longmsg = 'No missing phaseup SNR estimates for %s ' % ms.basename
        shortmsg = 'No missing phaseup SNR estimates'
    else:
        score = float(nexpected - nmissing) / nexpected
        longmsg = 'Missing phaseup SNR estimates for spws %s in %s ' % \
            (missing_spws, ms.basename)
        shortmsg = 'Missing phaseup SNR estimates'

    origin = pqa.QAOrigin(metric_name='score_missing_phaseup_snrs',
                          metric_score=nmissing,
                          metric_units='Number of spws with missing SNR measurements')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_poor_phaseup_solutions(ms, spwids, nphsolutions, min_nsolutions):
    """
    Score is the fraction of spws with poor phaseup solutions
    """
    # Compute the number of expected and poor SNR measurements
    nexpected = len(spwids)
    poor_spws = []
    for i in range(len(spwids)):
        if not nphsolutions[i]:
            poor_spws.append(spwids[i])
        elif nphsolutions[i] < min_nsolutions:
            poor_spws.append(spwids[i])
    npoor = len(poor_spws) 

    if nexpected <= 0:
        score = 0.0
        longmsg = 'No phaseup solutions for %s ' % ms.basename
        shortmsg = 'No phaseup solutions'
    elif npoor <= 0:
        score = 1.0
        longmsg = 'No poorly determined phaseup solutions for %s ' % ms.basename
        shortmsg = 'No poorly determined phaseup solutions'
    else:
        score = float(nexpected - npoor) / nexpected
        longmsg = 'Poorly determined phaseup solutions for spws %s in %s ' % \
            (poor_spws, ms.basename)
        shortmsg = 'Poorly determined phaseup solutions'

    origin = pqa.QAOrigin(metric_name='score_poor_phaseup_solutions',
                          metric_score=npoor,
                          metric_units='Number of poor phaseup solutions')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_missing_bandpass_snrs(ms, spwids, bpsolints):
    """
    Score is the fraction of spws with bandpass SNR estimates
    """

    # Compute the number of expected and missing SNR measurements
    nexpected = len(spwids)
    missing_spws = []
    for i in range(len(spwids)):
        if not bpsolints[i]:
            missing_spws.append(spwids[i])
    nmissing = len(missing_spws) 

    if nexpected <= 0:
        score = 0.0
        longmsg = 'No bandpass SNR estimates for %s ' % ms.basename
        shortmsg = 'No bandpass SNR estimates'
    elif nmissing <= 0:
        score = 1.0
        longmsg = 'No missing bandpass SNR estimates for %s ' % ms.basename
        shortmsg = 'No missing bandpass SNR estimates'
    else:
        score = float(nexpected - nmissing) / nexpected
        longmsg = 'Missing bandpass SNR estimates for spws %s in%s ' % \
            (missing_spws, ms.basename)
        shortmsg = 'Missing bandpass SNR estimates'

    origin = pqa.QAOrigin(metric_name='score_missing_bandpass_snrs',
                          metric_score=nmissing,
                          metric_units='Number of missing bandpass SNR estimates')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_poor_bandpass_solutions(ms, spwids, nbpsolutions, min_nsolutions):
    """
    Score is the fraction of spws with poor bandpass solutions
    """
    # Compute the number of expected and poor solutions
    nexpected = len(spwids)
    poor_spws = []
    for i in range(len(spwids)):
        if not nbpsolutions[i]:
            poor_spws.append(spwids[i])
        elif nbpsolutions[i] < min_nsolutions:
            poor_spws.append(spwids[i])
    npoor = len(poor_spws) 

    if nexpected <= 0:
        score = 0.0
        longmsg = 'No bandpass solutions for %s ' % ms.basename
        shortmsg = 'No bandpass solutions'
    elif npoor <= 0:
        score = 1.0
        longmsg = 'No poorly determined bandpass solutions for %s ' % ms.basename
        shortmsg = 'No poorly determined bandpass solutions'
    else:
        score = float(nexpected - npoor) / nexpected
        longmsg = 'Poorly determined bandpass solutions for spws %s in %s ' % (poor_spws, ms.basename)
        shortmsg = 'Poorly determined bandpass solutions'

    origin = pqa.QAOrigin(metric_name='score_missing_bandpass_snrs',
                          metric_score=npoor,
                          metric_units='Number of poor bandpass solutions')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_missing_phase_snrs(ms, spwids, snrs):
    """
    Score is the fraction of spws with SNR estimates
    """
    # Compute the number of expected and missing SNR measurements
    nexpected = len(spwids)
    missing_spws = []
    for i in range(len(spwids)):
        if not snrs[i]:
            missing_spws.append(spwids[i])
    nmissing = len(missing_spws) 

    if nexpected <= 0:
        score = 0.0
        longmsg = 'No gaincal SNR estimates for %s ' % ms.basename
        shortmsg = 'No gaincal SNR estimates'
    elif nmissing <= 0:
        score = 1.0
        longmsg = 'No missing gaincal SNR estimates for %s ' % ms.basename
        shortmsg = 'No missing gaincal SNR estimates'
    else:
        score = float(nexpected - nmissing) / nexpected
        longmsg = 'Missing gaincal SNR estimates for spws %s in %s ' % (missing_spws, ms.basename)
        shortmsg = 'Missing gaincal SNR estimates'

    origin = pqa.QAOrigin(metric_name='score_missing_phase_snrs',
                          metric_score=nmissing,
                          metric_units='Number of missing phase SNR estimates')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_poor_phase_snrs(ms, spwids, minsnr, snrs):
    """
    Score is the fraction of spws with poor snr estimates
    """
    # Compute the number of expected and poor solutions
    nexpected = len(spwids)
    poor_spws = []
    for i in range(len(spwids)):
        if not snrs[i]:
            poor_spws.append(spwids[i])
        elif snrs[i] < minsnr:
            poor_spws.append(spwids[i])
    npoor = len(poor_spws) 

    if nexpected <= 0:
        score = 0.0
        longmsg = 'No gaincal SNR estimates for %s ' % \
            ms.basename
        shortmsg = 'No gaincal SNR estimates'
    elif npoor <= 0:
        score = 1.0
        longmsg = 'No low gaincal SNR estimates for %s ' % \
            ms.basename
        shortmsg = 'No low gaincal SNR estimates'
    else:
        score = float(nexpected - npoor) / nexpected
        longmsg = 'Low gaincal SNR estimates for spws %s in %s ' % \
            (poor_spws, ms.basename)
        shortmsg = 'Low gaincal SNR estimates'

    origin = pqa.QAOrigin(metric_name='score_poor_phase_snrs',
                          metric_score=npoor,
                          metric_units='Number of poor phase SNR estimates')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_derived_fluxes_snr(ms, measurements):
    """
    Score the SNR of the derived flux measurements.
        1.0 if SNR > 20.0
        0.0 if SNR < 5.0
        linear scale between 0.0 and 1.0 in between
    """
    # Loop over measurements
    nmeasured = 0
    score = 0.0
    minscore = 1.0
    minsnr = None

    for _, value in measurements.iteritems():
        # Loop over the flux measurements
        for flux in value:
            fluxjy = flux.I.to_units(measures.FluxDensityUnits.JANSKY)
            uncjy = flux.uncertainty.I.to_units(measures.FluxDensityUnits.JANSKY)
            if fluxjy <= 0.0 or uncjy <= 0.0: 
                continue
            snr = fluxjy / uncjy
            minsnr = snr if minsnr is None else min(minsnr, snr)
            nmeasured += 1
            score1 = linear_score(float(snr), 5.0, 20.0, 0.0, 1.0)
            minscore = min(minscore, score1)
            score += score1

    if nmeasured > 0:
        score /= nmeasured

    if nmeasured == 0:
        score = 0.0
        longmsg = 'No derived fluxes for %s ' % ms.basename
        shortmsg = 'No derived fluxes'
    elif minscore >= 1.0:
        score = 1.0
        longmsg = 'No low SNR derived fluxes for %s ' % ms.basename
        shortmsg = 'No low SNR derived fluxes'
    else:
        longmsg = 'Low SNR derived fluxes for %s ' % ms.basename
        shortmsg = 'Low SNR derived fluxes'

    origin = pqa.QAOrigin(metric_name='score_derived_fluxes_snr',
                          metric_score=minsnr,
                          metric_units='Minimum SNR of derived flux measurement')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, vis=ms.basename, origin=origin)


@log_qa
def score_path_exists(mspath, path, pathtype):
    """
    Score the existence of the path
        1.0 if it exist
        0.0 if it does not
    """
    if os.path.exists(path):
        score = 1.0
        longmsg = 'The %s file %s for %s was created' % (pathtype, os.path.basename(path), os.path.basename(mspath))
        shortmsg = 'The %s file was created' % pathtype
    else:
        score = 0.0
        longmsg = 'The %s file %s for %s was not created' % (pathtype, os.path.basename(path), os.path.basename(mspath))
        shortmsg = 'The %s file was not created' % pathtype

    origin = pqa.QAOrigin(metric_name='score_path_exists',
                          metric_score=bool(score),
                          metric_units='Path exists on disk')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_file_exists(filedir, filename, filetype):
    """
    Score the existence of a products file
        1.0 if it exists
        0.0 if it does not
    """
    if filename is None:
        score = 1.0
        longmsg = 'The %s file is undefined' % filetype
        shortmsg = 'The %s file is undefined' % filetype

        origin = pqa.QAOrigin(metric_name='score_file_exists',
                              metric_score=None,
                              metric_units='No %s file to check' % filetype)

        return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, origin=origin)

    file_path = os.path.join(filedir, os.path.basename(filename))
    if os.path.exists(file_path):
        score = 1.0
        longmsg = 'The %s file has been exported' % filetype
        shortmsg = 'The %s file has been exported' % filetype
    else:
        score = 0.0
        longmsg = 'The %s file %s does not exist' % (filetype, os.path.basename(filename))
        shortmsg = 'The %s file does not exist' % filetype

    origin = pqa.QAOrigin(metric_name='score_file_exists',
                          metric_score=bool(score),
                          metric_units='File exists on disk')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_flags_exist(filedir, visdict):
    """
    Score the existence of the flagging products files 
        1.0 if they all exist
        n / nexpected if some of them exist
        0.0 if none exist
    """
    nexpected = len(visdict)
    nfiles = 0
    missing = []

    for visname in visdict:
        file_path = os.path.join(filedir, os.path.basename(visdict[visname][0]))
        if os.path.exists(file_path):
            nfiles += 1
        else:
            missing.append(os.path.basename(visdict[visname][0]))

    if nfiles <= 0:
        score = 0.0
        longmsg = 'Final flag version files %s are missing' % (','.join(missing))
        shortmsg = 'Missing final flags version files'
    elif nfiles < nexpected:
        score = float(nfiles) / float(nexpected)
        longmsg = 'Final flag version files %s are missing' % (','.join(missing))
        shortmsg = 'Missing final flags version files'
    else:
        score = 1.0
        longmsg = 'No missing final flag version files'
        shortmsg = 'No missing final flags version files'

    origin = pqa.QAOrigin(metric_name='score_flags_exist',
                          metric_score=len(missing),
                          metric_units='Number of missing flagging product files')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_applycmds_exist(filedir, visdict):
    """
    Score the existence of the apply commands products files
        1.0 if they all exist
        n / nexpected if some of them exist
        0.0 if none exist
    """
    nexpected = len(visdict)
    nfiles = 0
    missing = []

    for visname in visdict:
        file_path = os.path.join(filedir, os.path.basename(visdict[visname][1]))
        if os.path.exists(file_path):
            nfiles += 1
        else:
            missing.append(os.path.basename(visdict[visname][1]))

    if nfiles <= 0:
        score = 0.0
        longmsg = 'Final apply commands files %s are missing' % (','.join(missing))
        shortmsg = 'Missing final apply commands files'
    elif nfiles < nexpected:
        score = float(nfiles) / float(nexpected)
        longmsg = 'Final apply commands files %s are missing' % (','.join(missing))
        shortmsg = 'Missing final apply commands files'
    else:
        score = 1.0
        longmsg = 'No missing final apply commands files'
        shortmsg = 'No missing final apply commands files'

    origin = pqa.QAOrigin(metric_name='score_applycmds_exist',
                          metric_score=len(missing),
                          metric_units='Number of missing apply command files')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_caltables_exist(filedir, sessiondict):
    """
    Score the existence of the caltables products files
        1.0 if theu all exist
        n / nexpected if some of them exist
        0.0 if none exist
    """
    nexpected = len(sessiondict)
    nfiles = 0
    missing = []

    for sessionname in sessiondict:
        file_path = os.path.join(filedir, os.path.basename(sessiondict[sessionname][1]))
        if os.path.exists(file_path):
            nfiles += 1
        else:
            missing.append(os.path.basename(sessiondict[sessionname][1]))

    if nfiles <= 0:
        score = 0.0
        longmsg = 'Caltables files %s are missing' % (','.join(missing))
        shortmsg = 'Missing caltables files'
    elif nfiles < nexpected:
        score = float(nfiles) / float(nexpected)
        longmsg = 'Caltables files %s are missing' % (','.join(missing))
        shortmsg = 'Missing caltables files'
    else:
        score = 1.0
        longmsg = 'No missing caltables files'
        shortmsg = 'No missing caltables files'

    origin = pqa.QAOrigin(metric_name='score_caltables_exist',
                          metric_score=len(missing),
                          metric_units='Number of missing caltables')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_sd_line_detection(group_id_list, spw_id_list, lines_list):
    detected_spw = []
    detected_group = []

    for group_id, spw_id, lines in zip(group_id_list, spw_id_list, lines_list):
        if any([l[2] for l in lines]):
            LOG.trace('detected lines exist at group_id %s spw_id %s' % (group_id, spw_id))
            unique_spw_id = set(spw_id)
            if len(unique_spw_id) == 1:
                detected_spw.append(unique_spw_id.pop())
            else:
                detected_spw.append(-1)
            detected_group.append(group_id)

    if len(detected_spw) == 0:
        score = 0.0
        longmsg = 'No spectral lines were detected'
        shortmsg = 'No spectral lines were detected'
    else:
        score = 1.0
        if detected_spw.count(-1) == 0:
            longmsg = 'Spectral lines were detected in spws %s' % (', '.join(map(str, detected_spw)))
        else:
            longmsg = 'Spectral lines were detected in ReductionGroups %s' % (','.join(map(str, detected_group)))
        shortmsg = 'Spectral lines were detected'

    origin = pqa.QAOrigin(metric_name='score_sd_line_detection',
                          metric_score=len(detected_spw),
                          metric_units='Number of spectral lines detected')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_sd_line_detection_for_ms(group_id_list, field_id_list, spw_id_list, lines_list):
    detected_spw = []
    detected_field = []
    detected_group = []

    for group_id, field_id, spw_id, lines in zip(group_id_list, field_id_list, spw_id_list, lines_list):
        if any([l[2] for l in lines]):
            LOG.trace('detected lines exist at group_id %s field_id %s spw_id %s' % (group_id, field_id, spw_id))
            unique_spw_id = set(spw_id)
            if len(unique_spw_id) == 1:
                detected_spw.append(unique_spw_id.pop())
            else:
                detected_spw.append(-1)
            unique_field_id = set(field_id)
            if len(unique_field_id) == 1:
                detected_field.append(unique_field_id.pop())
            else:
                detected_field.append(-1)
            detected_group.append(group_id)

    if len(detected_spw) == 0:
        score = 0.0
        longmsg = 'No spectral lines are detected'
        shortmsg = 'No spectral lines are detected'
    else:
        score = 1.0
        if detected_spw.count(-1) == 0 and detected_field.count(-1) == 0:
            longmsg = 'Spectral lines are detected at Spws (%s) Fields (%s)' % (', '.join(map(str, detected_spw)),
                                                                                ', '.join(map(str, detected_field)))
        else:
            longmsg = 'Spectral lines are detected at ReductionGroups %s' % (','.join(map(str, detected_group)))
        shortmsg = 'Spectral lines are detected'

    origin = pqa.QAOrigin(metric_name='score_sd_line_detection_for_ms',
                          metric_score=len(detected_spw),
                          metric_units='Number of spectral lines detected')

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, origin=origin)


@log_qa
def score_checksources(mses, fieldname, spwid, imagename):
    """
    Score a single field image of a point source by comparing the source
    reference position to the fitted position and the source reference flux
    to the fitted flux.

    The source is assumed to be near the center of the image.
    The fit is performed using pixels in a circular regions
    around the center of the image
    """
    qa = casatools.quanta
    me = casatools.measures

    # Get the reference direction of the field
    #    Assume that the same field as defined by its field name
    #    has the same direction in all the mses that contributed
    #    to the input image. Loop through the ms(s) and find the
    #    first occurrence of the specified field. Convert the
    #    direction to ICRS to match the default image coordinate
    #    system

    refdirection = None
    for ms in mses:
        field = ms.get_fields(name=fieldname)
        if not field:
            continue
        if 'CHECK' not in field[0].intents:
            continue
        refdirection = me.measure(field[0].mdirection, 'ICRS')
        break

    # Get the reference flux of the field
    #    Loop over all the ms(s) extracting the derived flux
    #    values for the specified field and spw. Set the reference
    #    flux to the maximum of these values.
    reffluxes = []
    for ms in mses:
        if not ms.derived_fluxes:
            continue
        for field_arg, measurements in ms.derived_fluxes.items():
            mfield = ms.get_fields(field_arg)[0]
            if 'CHECK' not in mfield.intents:
                continue
            if mfield.name != fieldname:
                continue
            for measurement in sorted(measurements, key=lambda m: int(m.spw_id)):
                if int(measurement.spw_id) != spwid:
                    continue
                for stokes in ['I']:
                    try:
                        flux = getattr(measurement, stokes)
                        flux_jy = float(flux.to_units(measures.FluxDensityUnits.JANSKY))
                        reffluxes.append(flux_jy)
                    except:
                        pass

    # Use the maximum reference flux
    if not reffluxes:
        refflux = None
    else:
        median_flux = np.median(np.array(reffluxes))
        refflux = qa.quantity(median_flux, 'Jy')

    # Do the fit and compute positions offsets and flux ratios
    fitdict = checksource.checkimage(imagename, refdirection, refflux)

    # Compute the scores the default score is the geometric mean of
    # the position and flux scores if both are available.
    if not fitdict:
        score = 0.0
        longmsg = 'Check source fit failed for %s spwd %d' % (fieldname, spwid)
        shortmsg = 'Check source fit failed'
        metric_score = None
        metric_units = 'Check source fit failed'

    else:
        offset = fitdict['positionoffset']['value'] * 1000.0
        beams = fitdict['beamoffset']['value']
        fitflux = fitdict['fitflux']['value']
        shortmsg = 'Check source fit successful'
        if not refflux:
            score = max(0.0, 1.0 - min(1.0, beams))
            longmsg = ('Check source fit for %s spwd %d:  offet %0.3fmarcsec %0.3fbeams  fit flux %0.3fJy  '
                       'decoherence None' % (fieldname, spwid, offset, beams, fitflux))
            metric_score = beams
            metric_units = 'beams'

        else:
            coherence = fitdict['fluxloss']['value'] * 100.0
            offsetscore = max(0.0, 1.0 - min(1.0, beams))
            fluxscore = max(0.0, 1.0 - fitdict['fluxloss']['value'])
            score = math.sqrt(fluxscore * offsetscore)
            longmsg = ('Check source fit for %s spwd %d:  offet %0.3fmarcsec %0.3fbeams  fit flux %0.3fJy  '
                       'decoherence %0.3f percent' % (fieldname, spwid, offset, beams, fitflux, coherence))

            metric_score = (fluxscore, offsetscore)
            metric_units = 'flux score, offset score'

    origin = pqa.QAOrigin(metric_name='score_checksources',
                          metric_score=metric_score,
                          metric_units=metric_units)

    return pqa.QAScore(score, longmsg=longmsg, shortmsg=shortmsg, origin=origin)
