#!/usr/bin/env python

from glob import glob
import locale
import os

def lsms(musthave=[], mspat="*[-_.][Mm][Ss]", combine='or', remind=True):
    """
    Summarize measurement sets matching certain criteria.

    Arguments:

    musthave: A list of columns, subtables, or keywords that must be in the MS.
              If [] (default), the list of optional columns, subtables, and
              keywords of each MS will be printed.  Any entries will be
              internally uppercased.

    mspat:    A filename pattern, relative to the current directory, that the
              directory names matching of the MSes must match.
              Default: '*[-_.][Mm][Ss]'

    combine:  Controls whether the conditions of musthave are combined with
              'or' (default) or 'and'.

    remind:   If True (default), print all columns and keywords of optional
              subtables, not just the optional ones.

    Note that to fit in better with *sh behavior the argument order is reversed
    when calling from a non-python shell.  i.e. if you enter
      lsms \*_MS source polarization
    in a *sh session, it will run with
      mspat='*_MS' and musthave=['SOURCE', 'POLARIZATION'].
    (remember to quote wildcards to avoid sh expansion)
    """
    if type(musthave) == str:
        musthave = [s.replace(',', '') for s in musthave.split()]
    msdict, use_tb = matchingMSes(musthave, mspat, combine)
    mses = msdict.keys()
    
    # Do a locale sensitive sort of mses - this and some other niceties were
    # cribbed from an implementation by Padraig Brady of ls in python at
    # http://www.pixelbeat.org/talks/python/ls.py.html
    locale.setlocale(locale.LC_ALL, '')
    mses.sort(locale.strcoll)

    # have_colors, termwidth = termprops(sys.stdout)

    listall = True
    if musthave:
        listall = False

    for currms in mses:
        currmsstr = ''
        if listall:                # List all its optional things
            notindefn = []
            subtabs = msdict[currms].keys()
            subtabs.sort()
            for st in subtabs:
                ststr = ''
                if use_tb:
                    if st in mstables['req']:
                        optcols = set(msdict[currms][st]['cols']).difference(mstables['req'][st]['req']['cols'])
                        if optcols:
                            ststr = "    Optional column"
                            ststr += string_from_list_or_set(optcols)

                        optkws = msdict[currms][st]['kws'].difference(mstables['req'][st]['req']['kws'])
                        if optkws:
                            ststr += "    Optional keyword"
                            ststr += string_from_list_or_set(optkws)

                    elif st in mstables['opt']:
                        reqcols = mstables['opt'][st]['req']['cols']
                        if remind and reqcols:
                            ststr = '    Required column'
                            ststr += string_from_list_or_set(reqcols)

                        optcols = set(msdict[currms][st]['cols']).difference(reqcols)
                        if optcols:
                            ststr += "    Optional column"
                            ststr += string_from_list_or_set(optcols)

                        reqkws = mstables['opt'][st]['req']['kws']
                        if remind and reqkws:
                            ststr = '    Required keyword'
                            ststr += string_from_list_or_set(reqkws)

                        optkws = msdict[currms][st]['kws']
                        optkws.difference_update(reqkws)
                        if optkws:
                            ststr += "    Optional keyword"
                            ststr += string_from_list_or_set(optkws)

                        if not ststr:
                            currmsstr += "  " + st + "\n"
                    else:
                        notindefn.append(st)
                elif st not in mstables['req']:
                    notindefn.append(st)
                    
                if ststr:
                    currmsstr += "  " + st + ":\n" + ststr

            if notindefn:
                notindefn.sort()
                if use_tb:
                    currmsstr += "  Not in MS def'n V. 2.0: "
                currmsstr += ', '.join(notindefn) + "\n"                        

        if currmsstr:
            if use_tb:
                print currms + ":\n" + currmsstr
            else:
                print currms + ": " + currmsstr.strip()
        else:
            print currms

def string_from_list_or_set(li):
    retstr = ''
    if len(li) > 1:
        retstr += 's'
    li = list(li)
    li.sort()
    return retstr + ': ' + ', '.join(li) + "\n"

# From MS def'n v. 2, http://aips2.nrao.edu/docs/notes/229/229.html
mstables = {
    'req': {'MAIN':             {'req': {'cols': set(['TIME',
                                                      'ANTENNA1',
                                                      'ANTENNA2',
                                                      'FEED1',
                                                      'FEED2',
                                                      'DATA_DESC_ID',
                                                      'PROCESSOR_ID',
                                                      'FIELD_ID',
                                                      'INTERVAL',
                                                      'EXPOSURE',
                                                      'TIME_CENTROID',
                                                      'SCAN_NUMBER',
                                                      'ARRAY_ID',
                                                      'OBSERVATION_ID',
                                                      'STATE_ID',
                                                      'UVW',
                                                      'SIGMA',
                                                      'WEIGHT',
                                                      'FLAG',
                                                      'FLAG_CATEGORY',
                                                      'FLAG_ROW']),
                                         'kws':  set(['MS_VERSION'])},
                                 'opt': {'cols': set(['TIME_EXTRA_PREC',
                                                      'ANTENNA3',
                                                      'FEED3',
                                                      'PHASE_ID',
                                                      'PULSAR_BIN',
                                                      'PULSAR_GATE_ID',
                                                      'BASELINE_REF',
                                                      'UVW2',
                                                      'DATA',
                                                      'MODEL_DATA',
                                                      'CORRECTED_DATA',
                                                      'FLOAT_DATA',
                                                      'VIDEO_POINT',
                                                      'LAG_DATA',
                                                      'SIGMA_SPECTRUM',
                                                      'WEIGHT_SPECTRUM']),
                                         'kws':  set(['SORT_COLUMNS',
                                                      'SORT_ORDER'])}},
            'ANTENNA':          {'req': {'cols': set(['NAME',
                                                      'STATION',
                                                      'TYPE',
                                                      'MOUNT',
                                                      'POSITION',
                                                      'OFFSET',
                                                      'DISH_DIAMETER',
                                                      'FLAG_ROW']),
                                         'kws':  set([])},
                                 'opt': {'cols': set(['ORBIT_ID',
                                                      'MEAN_ORBIT',
                                                      'PHASED_ARRAY_ID']),
                                         'kws':  set([])}},
            'DATA_DESCRIPTION': {'req': {'cols': set(['SPECTRAL_WINDOW_ID',
                                                      'POLARIZATION_ID',
                                                      'FLAG_ROW']),
                                         'kws':  set([])},
                                 'opt': {'cols': set(['LAG_ID']),
                                         'kws':  set([])}},
            'FEED':             {'req': {'cols': set(['ANTENNA_ID',
                                                      'FEED_ID',
                                                      'SPECTRAL_WINDOW_ID',
                                                      'TIME',
                                                      'INTERVAL',
                                                      'NUM_RECEPTORS',
                                                      'BEAM_ID',
                                                      'BEAM_OFFSET',
                                                      'POLARIZATION_TYPE',
                                                      'POL_RESPONSE',
                                                      'POSITION',
                                                      'RECEPTOR_ANGLE']),
                                         'kws':  set([])},
                                 'opt': {'cols': set(['FOCUS_LENGTH',
                                                      'PHASED_FEED_ID']),
                                         'kws':  set([])}},
            'FIELD':            {'req': {'cols': set(['NAME',
                                                      'CODE',
                                                      'TIME',
                                                      'NUM_POLY',
                                                      'DELAY_DIR',
                                                      'PHASE_DIR',
                                                      'REFERENCE_DIR',
                                                      'SOURCE_ID',
                                                      'FLAG_ROW']),
                                         'kws':  set([])},
                                 'opt': {'cols': set(['EPHEMERIS_ID']),
                                         'kws':  set([])}},
            'FLAG_CMD':         {'req': {'cols': set(['TIME',
                                                      'INTERVAL',
                                                      'TYPE',
                                                      'REASON',
                                                      'LEVEL',
                                                      'SEVERITY',
                                                      'APPLIED',
                                                      'COMMAND']),
                                         'kws':  set([])},
                                 'opt': {'cols': set([]),
                                         'kws':  set([])}},
            'HISTORY':          {'req': {'cols': set(['TIME',
                                                      'OBSERVATION_ID',
                                                      'MESSAGE',
                                                      'PRIORITY',
                                                      'ORIGIN',
                                                      'OBJECT_ID',
                                                      'APPLICATION',
                                                      'CLI_COMMAND',
                                                      'APP_PARAMS']),
                                         'kws':  set([])},
                                 'opt': {'cols': set([]),
                                         'kws':  set([])}},
            'OBSERVATION':      {'req': {'cols': set(['TELESCOPE_NAME',
                                                      'TIME_RANGE',
                                                      'OBSERVER',
                                                      'LOG',
                                                      'SCHEDULE_TYPE',
                                                      'SCHEDULE',
                                                      'PROJECT',
                                                      'RELEASE_DATE',
                                                      'FLAG_ROW']),
                                         'kws':  set([])},
                                 'opt': {'cols': set([]),
                                         'kws':  set([])}},
            'POINTING':         {'req': {'cols': set(['ANTENNA_ID',
                                                      'TIME',
                                                      'INTERVAL',
                                                      'NAME',
                                                      'NUM_POLY',
                                                      'TIME_ORIGIN',
                                                      'DIRECTION',
                                                      'TARGET',
                                                      'TRACKING']),
                                         'kws':  set([])},
                                 'opt': {'cols': set(['POINTING_OFFSET',
                                                      'SOURCE_OFFSET',
                                                      'ENCODER',
                                                      'POINTING_MODEL_ID',
                                                      'ON_SOURCE',
                                                      'OVER_THE_TOP']),
                                         'kws':  set([])}},
            'POLARIZATION':     {'req': {'cols': set(['NUM_CORR',
                                                      'CORR_TYPE',
                                                      'CORR_PRODUCT',
                                                      'FLAG_ROW']),
                                         'kws':  set([])},
                                 'opt': {'cols': set([]),
                                         'kws':  set([])}},
            'PROCESSOR':        {'req': {'cols': set(['TYPE',
                                                      'SUB_TYPE',
                                                      'TYPE_ID',
                                                      'MODE_ID',
                                                      'FLAG_ROW']),
                                         'kws':  set([])},
                                 'opt': {'cols': set(['PASS_ID']),
                                         'kws':  set([])}},
            'SPECTRAL_WINDOW':  {'req': {'cols': set(['NUM_CHAN',
                                                      'NAME',
                                                      'REF_FREQUENCY',
                                                      'CHAN_FREQ',
                                                      'CHAN_WIDTH',
                                                      'MEAS_FREQ_REF',
                                                      'EFFECTIVE_BW',
                                                      'RESOLUTION',
                                                      'TOTAL_BANDWIDTH',
                                                      'NET_SIDEBAND',
                                                      'IF_CONV_CHAIN',
                                                      'FREQ_GROUP',
                                                      'FREQ_GROUP_NAME',
                                                      'FLAG_ROW']),
                                         'kws':  set([])},
                                 'opt': {'cols': set(['BBC_NO',
                                                      'BBC_SIDEBAND',
                                                      'RECEIVER_ID',
                                                      'DOPPLER_ID',
                                                      'ASSOC_SPW_ID',
                                                      'ASSOC_NATURE']),
                                         'kws':  set([])}},
            'STATE':            {'req': {'cols': set(['SIG',
                                                      'REF',
                                                      'CAL',
                                                      'LOAD',
                                                      'SUB_SCAN',
                                                      'OBS_MODE',
                                                      'FLAG_ROW']),
                                         'kws':  set([])},
                                 'opt': {'cols': set([]),
                                         'kws':  set([])}}
            },
    'opt': {'DOPPLER':          {'req': {'cols': set(['DOPPLER_ID',
                                                      'SOURCE_ID',
                                                      'TRANSITION_ID',
                                                      'VELDEF']),
                                         'kws':  set([])},
                                 'opt': {'cols': set([]),
                                         'kws':  set([])}},
            'FREQ_OFFSET':      {'req': {'cols': set(['ANTENNA1',
                                                      'ANTENNA2',
                                                      'FEED_ID',
                                                      'SPECTRAL_WINDOW_ID',
                                                      'TIME',
                                                      'INTERVAL',
                                                      'OFFSET']),
                                         'kws':  set([])},
                                 'opt': {'cols': set([]),
                                         'kws':  set([])}},
            'SOURCE':           {'req': {'cols': set(['SOURCE_ID',
                                                      'TIME',
                                                      'INTERVAL',
                                                      'SPECTRAL_WINDOW_ID',
                                                      'NUM_LINES',
                                                      'NAME',
                                                      'CALIBRATION_GROUP',
                                                      'CODE',
                                                      'DIRECTION',
                                                      'PROPER_MOTION']),
                                         'kws':  set([])},
                                 'opt': {'cols': set(['POSITION',
                                                      'TRANSITION',
                                                      'REST_FREQUENCY',
                                                      'SYSVEL',
                                                      'SOURCE_MODEL',
                                                      'PULSAR_ID']),
                                         'kws':  set([])}},
            'SYSCAL':           {'req': {'cols': set(['ANTENNA_ID',
                                                      'FEED_ID',
                                                      'SPECTRAL_WINDOW_ID',
                                                      'TIME',
                                                      'INTERVAL']),
                                         'kws':  set([])},
                                 'opt': {'cols': set(['PHASE_DIFF',
                                                      'TCAL',
                                                      'TRX',
                                                      'TSKY',
                                                      'TSYS',
                                                      'TANT',
                                                      'TANT_TSYS',
                                                      'TCAL_SPECTRUM',
                                                      'TRX_SPECTRUM',
                                                      'TSKY_SPECTRUM',
                                                      'TSYS_SPECTRUM',
                                                      'TANT_SPECTRUM',
                                                      'TANT_TSYS_SPECTRUM',
                                                      'PHASE_DIFF_FLAG',
                                                      'TCAL_FLAG',
                                                      'TRX_FLAG',
                                                      'TSKY_FLAG',
                                                      'TSYS_FLAG',
                                                      'TANT_FLAG',
                                                      'TANT_TSYS_FLAG']),
                                         'kws':  set([])}},
            'WEATHER':          {'req': {'cols': set(['ANTENNA_ID',
                                                      'TIME',
                                                      'INTERVAL']),
                                         'kws':  set([])},
                                 'opt': {'cols': set(['H2O',
                                                      'IONOS_ELECTRON',
                                                      'PRESSURE',
                                                      'REL_HUMIDITY',
                                                      'TEMPERATURE',
                                                      'DEW_POINT',
                                                      'WIND_DIRECTION',
                                                      'WIND_SPEED',
                                                      'H2O_FLAG',
                                                      'IONOS_ELECTRON_FLAG',
                                                      'PRESSURE_FLAG',
                                                      'REL_HUMIDITY_FLAG',
                                                      'TEMPERATURE_FLAG',
                                                      'DEW_POINT_FLAG',
                                                      'WIND_DIRECTION_FLAG',
                                                      'WIND_SPEED_FLAG']),
                                         'kws':  set([])}}
            }
    }

possible_subtables = set(mstables['req'].keys() + mstables['opt'].keys())

def matchingMSes(musthave=[], mspat="*.ms", combine='or'):
    """
    Returns a dict of MSes that match musthave and mspat as in
    lsms(musthave, mspat, combine), and whether or not it found the tb tool.
    """
    retval = {}

    use_and = False
    if(combine.lower() == 'and'):
        use_and = True

    musthave = [s.upper() for s in musthave]
    musthave = set(musthave)
    listall = False
    if not musthave:
        listall = True
        use_and = False

    needed_subtables = musthave.intersection(possible_subtables)
    needed_items = {'anywhere': set([])}  # cols and keywords
    for mh in musthave:
        mhparts = mh.split('/')
        if len(mhparts) > 1:
            if not needed_items.has_key(mhparts[0]):
                needed_items[mhparts[0]] = set([mhparts[1]])
            else:
                needed_items.add(mhparts[1])           
            if mhparts[0] != 'MAIN':
                needed_subtables.add(mhparts[0])
        elif mh not in possible_subtables:
            needed_items['anywhere'].add(mh)

    use_tb = False
    need_tb = musthave.difference(needed_subtables)
    if need_tb or listall:
        try:
            use_tb = hasattr(tb, 'colnames')
        except:
            try:
                #import sys
                ## sys.path.append(os.environ["CASAPATH"].split()[0] +
                ##                 '/code/xmlcasa/scripts/recipes')
                ## from taskutil import get_global_namespace
                ## my_globals = get_global_namespace()
                ## tb = my_globals['tb']
                from casa import table as tb
                use_tb = hasattr(tb, 'colnames')
            except:
                print "Could not find the tb tool.  Try running inside a casapy session or setting PYTHONPATH to /usr/lib/casapy/.../lib/python2.5."
        if need_tb and not use_tb:
            print "Removing", ', '.join(need_tb), "from the criteria for matching."
            musthave.difference_update(need_tb)
    
    mses = glob(mspat)
    for currms in mses:
        retval[currms] = {'MAIN': {}}
        keep_currms = listall

        subtabs = glob(currms + '/[A-Z]*')
        subtabs = set([s.replace(currms + '/', '', 1) for s in subtabs])
        for s in subtabs:
            retval[currms][s] = {}

        if needed_subtables:
            if use_and:
                keep_currms = needed_subtables.issubset(subtabs)
            elif needed_subtables.intersection(subtabs):
                keep_currms = True

        if use_tb and (keep_currms or listall or (not use_and)):
            subtabs_to_check = needed_subtables
            if listall or needed_items['anywhere']:
                subtabs_to_check = subtabs

            if listall or needed_items['anywhere'] or needed_items['MAIN']:
                # Start with MAIN
                tb.open(currms)
                retval[currms]['MAIN']['cols'] = tb.colnames()
                kws = set(tb.keywordnames())
                retval[currms]['MAIN']['kws'] = kws.difference(possible_subtables)
                tb.close()

                if not listall:
                    mainitems = set(retval[currms]['MAIN']['cols'])
                    mainitems.update(retval[currms]['MAIN']['kws'])
                    if use_and:
                        keep_currms = needed_items['MAIN'].issubset(mainitems)
                    elif not keep_currms:
                        my_needed_items = set(needed_items.get('MAIN', []))
                        my_needed_items.update(needed_items.get('anywhere', []))
                        if my_needed_items.intersection(mainitems):
                            keep_currms = True

            for st in subtabs_to_check:
                stdir = currms + '/' + st
                if os.path.isdir(stdir):
                    tb.open(stdir)
                    retval[currms][st]['cols'] = tb.colnames()
                    retval[currms][st]['kws'] = set(tb.keywordnames())
                    tb.close()
                    if not listall:
                        stitems = set(retval[currms][st]['cols'])
                        stitems.update(retval[currms][st]['kws'])
                        if use_and:
                            keep_currms = needed_items[st].issubset(stitems)
                            if not keep_currms:
                                break
                        elif not keep_currms:
                            my_needed_items = set(needed_items.get(st, []))
                            my_needed_items.update(needed_items.get('anywhere', []))
                            if my_needed_items.intersection(stitems):
                                keep_currms = True
                elif st in needed_subtables:
                    keep_currms = False
                    break
            
        if not keep_currms:
            del retval[currms]
        
    return retval, use_tb

# following, sort of, from Python cookbook, #475186
def termprops(stream):
    """
    Return whether or not stream supports colors, and a guess at its number of
    columns (in characters).
    """
    have_colors = False
    termwidth   = 80
    if hasattr(stream, "isatty") and stream.isatty():
        try:
            import curses
            curses.setupterm()
            termwidth = curses.tigetnum('cols')
            if curses.tigetnum("colors") > 2:
                have_colors = True
        except:
            pass
    return have_colors, termwidth

# Even more things came from a similar ls in python by C. Blake,
# http://pdos.csail.mit.edu/~cblake/cls/cls.py
# (I did do this in perl as a postdoc.)

def rowscols(n, nc):                    ### handle ceil(n/nc) assignment
    div, mod = divmod(n, nc)
    return div + (mod != 0), nc

if __name__ == '__main__':
    import sys
    mspat = '*.ms'
    musthave = []
    if len(sys.argv) > 1:
        mspat = sys.argv[1]
        musthave = sys.argv[2:]
    lsms(musthave, mspat)
