from __future__ import absolute_import
import os
import re

import pipeline.infrastructure as infrastructure
import pipeline.infrastructure.callibrary as callibrary
import pipeline.domain as domain
from pipeline.infrastructure import sdtablereader
from ... import heuristics
from .. import common
from ..common import utils

import asap as sd

LOG = infrastructure.get_logger(__name__)


class SDImportDataInputs(common.SingleDishInputs):
    def __init__(self, context=None, infiles=None, output_dir=None, 
                 session=None, overwrite=None):
        self._init_properties(vars())

    @property
    def overwrite(self):
        return self._overwrite

    @overwrite.setter
    def overwrite(self, value):
        if value is None:
            value = False
        else:
            value = utils.to_bool(value)
        self._overwrite = value

    def to_casa_args(self):
        raise NotImplementedError

    @property
    def infiles(self):
        return self._infiles
 
    @infiles.setter
    def infiles(self, value):
        if value is None:
            self._infiles = []
        else:
            self._infiles = value

class SDImportDataResults(common.SingleDishResults):
    def __init__(self, mses=[], scantables=[]):
        super(SDImportDataResults, self).__init__()
        self.mses = mses
        self.scantables = scantables
        
    def merge_with_context(self, context):
        if not isinstance(context.observing_run, domain.ScantableList):
            context.observing_run = domain.ScantableList()
            context.callibrary = callibrary.SDCalLibrary(context)
        target = context.observing_run
        for ms in self.mses:
            LOG.info('Adding {0} to context'.format(ms.name))
            target.add_measurement_set(ms)
        for st in self.scantables:
            LOG.info('Adding {0} to context'.format(st.name))            
            target.add_scantable(st)
            
    def __repr__(self):
        return 'SDImportDataResults:\n\t{0}\n\t{1}'.format(
            '\n\t'.join([ms.name for ms in self.mses]),
            '\n\t'.join([st.name for st in self.scantables]))


class SDImportData(common.SingleDishTaskTemplate):
    Inputs = SDImportDataInputs

    def _ms_directories(self, names):
        """
        Inspect a list of file entries, finding the root directory of any
        measurement sets present via a set of identifying files and
        directories.
        """
        identifiers = ('SOURCE', 'FIELD', 'ANTENNA', 'DATA_DESCRIPTION')
        
        matching = [os.path.dirname(n) for n in names 
                    if os.path.basename(n) in identifiers]

        return set([m for m in matching 
                    if matching.count(m) == len(identifiers)])

    def prepare(self, **parameters):
        inputs = self.inputs
        infiles = inputs.infiles
        vis = [v.name for v in inputs.context.observing_run.measurement_sets]
        to_import = set(vis)

        to_import_sd = set()
        to_convert_sd = set()


        if True:
            # get a list of all the files in the given directory
            h = heuristics.DataTypeHeuristics()
            for v in infiles:
                data_type = h(v).upper()
                if data_type == 'ASDM' or data_type == 'MS2':
                    raise TypeError, '%s should not be an ASDM nor MS'%(v)
                elif data_type == 'ASAP':
                    to_import_sd.add(v)
                elif data_type in ['FITS','NRO']:
                    to_convert_sd.add(v)

            to_import_sd = map(os.path.abspath, to_import_sd)

        if self._executor._dry_run:
            return SDImportDataResults()

        ms_reader = sdtablereader.ObservingRunReader

        # convert MS to Scantable
        scantable_list = []
        st_ms_map = []
        idx = 0
        for ms in to_import:
            LOG.info('Import %s as Scantable'%(ms))
            prefix = ms.rstrip('/')
            if re.match('.*\.(ms|MS)$',prefix):
                prefix = prefix[:-3]
            LOG.debug('prefix: %s'%(prefix))
            scantables = sd.splitant(filename=ms,#ms.name,
                                     outprefix=prefix,
                                     overwrite=True)
            scantable_list.extend(scantables)
            st_ms_map.extend([idx]*len(scantables))
            
            idx += 1
        
        # launch an import job for each non-Scantable, non-MS single-dish data
        for any_data in to_convert_sd:
            LOG.info('Importing %s as Scantable'%(any_data))
            self._import_to_scantable(any_data)
            
        # calculate the filenames of the resultant Scantable
        to_import_sd.extend([self._any_data_to_scantable_name(name) for name in to_convert_sd])

        # There are no associating MS data for non-MS inputs
        scantable_list.extend(to_import_sd)
        if len(st_ms_map) > 0:
            st_ms_map.extend([-1]*len(to_import_sd))

        # Now everything is in Scnatable format, import them
        LOG.debug('scantable_list=%s'%(scantable_list))
        LOG.debug('to_import=%s'%(to_import))
        LOG.debug('st_ms_map=%s'%(st_ms_map))
        observing_run_sd = ms_reader.get_observing_run_for_sd(scantable_list,
                                                              to_import,
                                                              st_ms_map)
    
        for st in observing_run_sd:
            LOG.debug('Setting session to %s for %s' % (inputs.session, 
                                                        st.basename))
            st.session = inputs.session
            
        for ms in observing_run_sd.measurement_sets:
            LOG.debug('Setting session to %s for %s' % (inputs.session,
                                                        ms.basename))
            ms.session = inputs.session

        results = SDImportDataResults(observing_run_sd.measurement_sets,
                                      observing_run_sd)
        return results
    
    def analyse(self, results):
        return results

    def _import_to_scantable(self, data):
        outfile = self._any_data_to_scantable_name(data)
        s = sd.scantable(data, avearge=False)
        s.save(outfile, format='ASAP', overwrite=self.inputs.overwrite)
        

    def _any_data_to_scantable_name(self, name):
        return '{0}.asap'.format(os.path.join(self.inputs.output_dir,
                                              os.path.basename(name.rstrip('/'))))
