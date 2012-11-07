import os
from taskinit import *

import asap as sd
from asap._asap import Scantable
import pylab as pl
from asap import _to_list
from asap.scantable import is_scantable
import sdutil

def sdcal(infile, antenna, fluxunit, telescopeparm, specunit, frame, doppler, calmode, fraction, noff, width, elongated, markonly, plotpointings, scanlist, field, iflist, pollist, channelrange, scanaverage, timeaverage, tweight, averageall, polaverage, pweight, tau, verify, outfile, outform, overwrite, plotlevel):
    
    casalog.origin('sdcal')
    
    ###
    ### Now the actual task code
    ###
    
    restorer = None
    
    try:
        worker = sdcal_worker(**locals())
        worker.initialize()
        worker.execute()
        worker.finalize()
        
    except Exception, instance:
        sdutil.process_exception(instance)
        raise Exception, instance

class sdcal_worker(sdutil.sdtask_template):
    def __init__(self, **kwargs):
        super(sdcal_worker,self).__init__(**kwargs)
        self.suffix = '_cal'

    def initialize_scan(self):
        sorg=sd.scantable(self.infile,average=False,antenna=self.antenna)

        if not isinstance(sorg,Scantable):
            raise Exception, 'Scantable data %s, is not found'

        # A scantable selection
        sel = self.get_selector()
        sorg.set_selection(sel)
        
        # Copy scantable when usign disk storage not to modify
        # the original table.
        if is_scantable(self.infile) and self.is_disk_storage:
            self.scan = sorg.copy()
        else:
            self.scan = sorg
        del sorg

    def execute(self):
        engine = sdcal_engine(self)
        engine.prologue()
        
        # apply inputs to scan
        self.set_to_scan()

##         # do opacity (atmospheric optical depth) correction
##         sdutil.doopacity(self.scan, self.tau)

##         # channel splitting
##         sdutil.dochannelrange(self.scan, self.channelrange)

        # Actual implementation is defined outside the class
        # since those are used in task_sdreduce.
        engine.drive()
        self.scan = engine.get_result()
        
        # do opacity (atmospheric optical depth) correction
        sdutil.doopacity(self.scan, self.tau)

        # channel splitting
        sdutil.dochannelrange(self.scan, self.channelrange)

        # Average data if necessary
        self.scan = sdutil.doaverage(self.scan, self.scanaverage, self.timeaverage, self.tweight, self.polaverage, self.pweight, self.averageall)

        engine.epilogue()

    def save(self):
        sdutil.save(self.scan, self.project, self.outform, self.overwrite)
        

class sdcal_engine(sdutil.sdtask_engine):
    def __init__(self, worker):
        super(sdcal_engine,self).__init__(worker)

    def prologue(self):
        if ( abs(self.plotlevel) > 1 ):
            casalog.post( "Initial Raw Scantable:" )
            self.worker.scan._summary()

    def drive(self):
        scanns = self.worker.scan.getscannos()
        sn=list(scanns)
        casalog.post( "Number of scans to be processed: %d" % (len(sn)) )
        if self.calmode == 'otf' or self.calmode=='otfraster':
            self.__mark()
            if not self.markonly:
                self.result = sd.asapmath.calibrate( self.result,
                                                     scannos=sn,
                                                     calmode='ps',
                                                     verify=self.verify )
        else:
            self.result = sd.asapmath.calibrate( self.worker.scan,
                                                 scannos=sn,
                                                 calmode=self.calmode,
                                                 verify=self.verify )

    def epilogue(self):
        if ( abs(self.plotlevel) > 1 ):
            casalog.post( "Final Calibrated Scantable:" )
            self.worker.scan._summary()

        # Plot final spectrum
        if ( abs(self.plotlevel) > 0 ):
            pltfile = project + '_calspec.eps'
            sdutil.plot_scantable(self.worker.scan, pltfile, self.plotlevel)

    def __mark(self):
        israster = (self.calmode == 'otfraster')
        marker = sd.edgemarker(israster=israster)
        marker.setdata(self.worker.scan)
        self.npts = self.noff
        marker.setoption(**self.__dict__)
        marker.mark()
        if self.plotpointings:
            marker.plot()
        self.result = marker.getresult()
        
