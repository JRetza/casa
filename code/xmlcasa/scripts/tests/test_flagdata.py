import shutil
import unittest
import os
from tasks import *
#from taskinit import *

#
# Test of flagdata modes
#

def test_eq(result, total, flagged):

    print "%s of %s data was flagged, expected %s of %s" % \
    (result['flagged'], result['total'], flagged, total)
    assert result['total'] == total, \
               "%s data in total; %s expected" % (result['total'], total)
    assert result['flagged'] == flagged, \
           "%s flags set; %s expected" % (result['flagged'], flagged)


# Base class which defines setUp functions
# for importing different data sets
class test_base(unittest.TestCase):
    def setUp_flagdatatest(self):
        self.vis = "flagdatatest.ms"

        if os.path.exists(self.vis):
            print "The MS is already around, just unflag"
        else:
            print "Moving data..."
            os.system('cp -r ' + \
                      os.environ.get('CASAPATH').split()[0] +
                      "/data/regression/unittest/flagdata/" + self.vis + ' ' + self.vis)

        os.system('rm -rf ' + self.vis + '.flagversions')
        flagdata(vis=self.vis, unflag=True)

    def setUp_ngc5921(self):
        self.vis = "ngc5921.ms"

        if os.path.exists(self.vis):
            print "The MS is already around, just unflag"
        else:
            print "Importing data..."
            importuvfits(os.environ.get('CASAPATH').split()[0] + \
                         '/data/regression/ngc5921/ngc5921.fits', \
                         self.vis)
            
        os.system('rm -rf ' + self.vis + '.flagversions')
        flagdata(vis=self.vis, unflag=True)


class test_rfi(test_base):
    """Test of mode = 'rfi'"""
    
    def setUp(self):
        self.setUp_flagdatatest()
        
    def test1(self):
        flagdata(vis=self.vis, unflag=True)
        flagdata(vis=self.vis, mode='rfi')
        test_eq(flagdata(vis=self.vis, mode='summary'), 70902, 1326)
        test_eq(flagdata(vis=self.vis, mode='summary', antenna='2'), 5252, 51)
        flagdata(vis=self.vis, unflag=True)


class test_shadow(test_base):
    """Test of mode = 'shadow'"""
    def setUp(self):
        self.setUp_flagdatatest()

    def test1(self):
        flagdata(vis=self.vis, mode='shadow', diameter=40)
        test_eq(flagdata(vis=self.vis, mode='summary'), 70902, 5252)

    def test2(self):
        flagdata(vis=self.vis, mode='shadow')
        test_eq(flagdata(vis=self.vis, mode='summary'), 70902, 2912)

    def test3(self):
        flagdata(vis=self.vis, mode='shadow', correlation='LL')
        test_eq(flagdata(vis=self.vis, mode='summary'), 70902, 1456)


class test_vector_flagmanager(test_base):
    def runTest(self):

        # Import data
        vis = 'flagdatatest.ms'
        
        os.system('cp -r ' + \
                  os.environ.get('CASAPATH').split()[0] +
                  "/data/regression/unittest/flagdata/" + vis + ' ' + vis)

        print "Test of vector mode"
        
        default(flagdata)
        vis = 'flagdatatest.ms'

        flagdata(vis=vis, unflag=True)

        clipminmax=[0.0, 0.2]
        antenna = ['1', '2']
        clipcolumn = ['DATA', 'datA']

        inp(flagdata) # broken, looks only at global parameters
        flagdata(vis=vis, clipminmax=clipminmax, antenna=antenna, clipcolumn=clipcolumn)
        test_eq(flagdata(vis=vis, mode='summary'), 70902, 17897)
        flagdata(vis=vis, unflag=True)

        print "Test of flagmanager mode=list, flagbackup=True/False"
        flagmanager(vis=vis, mode='list')
        fg.open(vis)
        assert len(fg.getflagversionlist()) == 5
        fg.done()

        flagdata(vis=vis, unflag=True, flagbackup=False)
        flagmanager(vis=vis, mode='list')
        fg.open(vis)
        assert len(fg.getflagversionlist()) == 5
        fg.done()

        flagdata(vis=vis, unflag=True, flagbackup=True)
        flagmanager(vis=vis, mode='list')
        fg.open(vis)
        len(fg.getflagversionlist()) == 6
        fg.done()

        print "Test of flagmanager mode=rename"
        flagmanager(vis=vis, mode='rename', oldname='manualflag_3', versionname='Ha! The best version ever!', comment='This is a *much* better name')
        flagmanager(vis=vis, mode='list')
        fg.open(vis)
        len(fg.getflagversionlist()) == 6
        fg.done()

class test_flagmanager(test_base):

    def setUp(self):
        self.setUp_ngc5921()

    def test1(self):
        """Create, then restore autoflag"""

        flagdata(vis = self.vis, mode='summary')
        flagmanager(vis = self.vis)
        
        flagdata(vis = self.vis, antenna="2")
        
        flagmanager(vis = self.vis)
        ant2 = flagdata(vis = self.vis, mode='summary')['flagged']

        print "After flagging antenna 2 there were", ant2, "flags"

        # Change flags, then restore
        flagdata(vis = self.vis, antenna="3")
        flagmanager(vis = self.vis)
        ant3 = flagdata(vis = self.vis, mode='summary')['flagged']

        print "After flagging antenna 2 and 3 there were", ant3, "flags"

        flagmanager(vis = self.vis, mode='restore', versionname='manualflag_3')
        restore2 = flagdata(vis = self.vis, mode='summary')['flagged']

        print "After restoring pre-antenna 3 flagging, there are", restore2, "flags, should be", ant2

        assert restore2 == ant2


class test_statistics_queries(test_base):

    def setUp(self):
        self.setUp_ngc5921()

    def test_CAS2021(self):
        print "Test antenna selection"
        flagdata(vis=self.vis, antenna='!5') # should not crash

    def test_CAS2212(self):
        print "Test scan + clipping"
        flagdata(vis=self.vis, scan="2", clipminmax = [0.2, 0.3]) # should not crash
    
    def test021(self):
        print "Test of flagging statistics and queries"
        
        flagdata(vis=self.vis, correlation='LL')
        flagdata(vis=self.vis, spw='0:17~19')
        flagdata(vis=self.vis, antenna='5&&9')
        flagdata(vis=self.vis, antenna='14')
        flagdata(vis=self.vis, field='1')
        s = flagdata(vis=self.vis, mode='summary', minrel=0.9)
        assert s['antenna'].keys() == ['14']
        assert '5&&9' in s['baseline'].keys()
        assert set(s['channel'].keys()) == set(['17', '18', '19'])
        assert s['correlation'].keys() == ['1']  # LL
        assert s['field'].keys() == ['1']
        assert set(s['scan'].keys()) == set(['2', '4', '5', '7']) # field 1
        s = flagdata(vis=self.vis, mode='summary', maxrel=0.8)
        assert set(s['field'].keys()) == set(['0', '2'])
        s = flagdata(vis=self.vis, mode='summary', minabs=400000)
        assert set(s['scan'].keys()) == set(['3', '6'])
        s = flagdata(vis=self.vis, mode='summary', minabs=400000, maxabs=450000)
        assert s['scan'].keys() == ['3']

    def test2(self):
        print "Test of autoflag, algorithm=timemed"
        flagdata(vis=self.vis, mode='autoflag', algorithm='timemed', window=3)
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 4725)

    def test3(self):
        print "Test of autoflag, algorithm=freqmed"
        flagdata(vis=self.vis, mode='autoflag', algorithm='freqmed')
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 28916)

    def test4(self):
        print "Test of channel average"
        flagdata(vis=self.vis, channelavg=False, clipminmax=[30, 60])
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 1414186)

    def test5(self):
        flagdata(vis=self.vis, channelavg=True, clipminmax=[30, 60])
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 1347822)

    def test6(self):
        flagdata(vis=self.vis, channelavg=False, clipminmax=[30, 60], spw='0:0~10')
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 242053)

    def test7(self):
        flagdata(vis=self.vis, channelavg=True, clipminmax=[30, 60], spw='0:0~10')
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 231374)
               

    def test8(self):
        print "Test of mode = 'quack'"
        print "parallel quack"
        flagdata(vis=self.vis, mode='quack', quackinterval=[1.0, 5.0], antenna=['2', '3'], correlation='RR')
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 22365)

    def test9(self):
        flagdata(vis=self.vis, mode='quack', quackmode='beg', quackinterval=1)
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 329994)

    def test10(self):
        flagdata(vis=self.vis, mode='quack', quackmode='endb', quackinterval=1)
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 333396)

    def test11(self):
        flagdata(vis=self.vis, mode='quack', quackmode='end', quackinterval=1)
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 2520882)

    def test12(self):
        flagdata(vis=self.vis, mode='quack', quackmode='tail', quackinterval=1)
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 2524284)

    def test13(self):
        print "quack mode quackincrement"
        flagdata(vis=self.vis, mode='quack', quackinterval=50, quackmode='endb', quackincrement=True)
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 571536)

        flagdata(vis=self.vis, mode='quack', quackinterval=20, quackmode='endb', quackincrement=True)
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 857304)
        
        flagdata(vis=self.vis, mode='quack', quackinterval=150, quackmode='endb', quackincrement=True)
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 1571724)
        
        flagdata(vis=self.vis, mode='quack', quackinterval=50, quackmode='endb', quackincrement=True)
        test_eq(flagdata(vis=self.vis, mode='summary'), 2854278, 1762236)
        flagdata(vis=self.vis, unflag=True)



class test_selections(test_base):
    """Test various selections"""

    def setUp(self):
        self.setUp_ngc5921()

    def test_scan(self):
        
        flagdata(vis=self.vis, scan='3')
        test_eq(flagdata(vis=self.vis, mode='summary', antenna='2'), 196434, 52416)
        
        # feed not implemented flagdata(vis=vis, feed='27')
        # flagdata(vis=vis, unflag=True)

    def test_antenna(self):

        flagdata(vis=self.vis, antenna='2')
        test_eq(flagdata(vis=self.vis, mode='summary', antenna='2'), 196434, 196434)

    def test_spw(self):
        
        flagdata(vis=self.vis, spw='0')
        test_eq(flagdata(vis=self.vis, mode='summary', antenna='2'), 196434, 196434)

    def test_correlation(self):
        flagdata(vis=self.vis, correlation='LL')
        test_eq(flagdata(vis=self.vis, mode='summary', antenna='2'), 196434, 98217)
        flagdata(vis=self.vis, correlation='LL,RR')
        flagdata(vis=self.vis, correlation='LL RR')
        flagdata(vis=self.vis, correlation='LL ,, ,  ,RR')
        test_eq(flagdata(vis=self.vis, mode='summary', antenna='2'), 196434, 196434)

    def test_field(self):
        
        flagdata(vis=self.vis, field='0')
        test_eq(flagdata(vis=self.vis, mode='summary', antenna='2'), 196434, 39186)

    def test_uvrange(self):
        
        flagdata(vis=self.vis, uvrange='200~400m')
        test_eq(flagdata(vis=self.vis, mode='summary', antenna='2'), 196434, 55944)

    def test_timerange(self):
    
        flagdata(vis=self.vis, timerange='09:50:00~10:20:00')
        test_eq(flagdata(vis=self.vis, mode='summary', antenna='2'), 196434, 6552)

    def test_array(self):
        flagdata(vis=self.vis, array='0')
        test_eq(flagdata(vis=self.vis, mode='summary', antenna='2'), 196434, 196434)

# Dummy class which cleans up created files
class cleanup(test_base):
    
    def tearDown(self):
        os.system('rm -rf ngc5921.ms')
        os.system('rm -rf ngc5921.ms.flagversions')
        os.system('rm -rf flagdatatest.ms')
        os.system('rm -rf flagdatatest.ms.flagversions')
        
    def test1(self):
        '''flagdata: Cleanup'''
        pass


def suite():
    return [test_selections,
            test_statistics_queries,
            test_vector_flagmanager,
            test_flagmanager,
            test_rfi,
            test_shadow,
            cleanup]
