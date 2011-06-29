import os
import sys
import shutil
from __main__ import default
from tasks import *
from taskinit import *
import unittest
import time
from simple_cluster import *
import glob

class simplecluster_test(unittest.TestCase):
    # Input and output names
    
    projectname="test_simplecluster"
    resultfile="test_simplecluster.result"
    clusterfile="test_simplecluster"
    
    cluster=simple_cluster()

    def _cleanUp(self):
        if os.path.exists(self.resultfile):
            os.remove(self.resultfile)
        logfiles=glob.glob("engine-*.log")
        for i in logfiles:
            os.remove(i)
        if os.path.exists(self.clusterfile):
            os.remove(self.clusterfile)

    def setUp(self):
        self._cleanUp()

    def tearDown(self):
        #print 'tearDown.....get called when each test finish'
        self.cluster.cold_start()
        self._cleanUp()

    def test000(self):
        '''Test 0: create a default cluster'''
        host=os.uname()[1]
        cwd=os.getcwd()
        import multiprocessing
        ncpu=multiprocessing.cpu_count()
        msg=host+', '+str(ncpu)+', '+cwd
        f=open(self.clusterfile, 'w')
        f.write(msg)
        f.close()
        self._waitForFile(self.clusterfile, 10)

        self.cluster.init_cluster(self.clusterfile, self.projectname)

        print self.cluster._cluster.hello()
        print "engines:", self.cluster.use_engines()
        print "hosts:",  self.cluster.get_hosts()

    def _checkResultFile(self):
        self.assertTrue(os.path.isfile(self.resultfile))
            
    def _waitForFile(self, file, seconds):
        for i in range(0,seconds):
            if (os.path.isfile(file)):
                return
            time.sleep(1)

class testJobData(unittest.TestCase):
    '''
    This class tests the JobData class in the simple_cluster.
    '''

    def testSimpleJob(self):
        jd = JobData('myJob')
        self.assertEqual('myJob()', jd.getCommandLine())

    def testJobWithOneArg(self):
        jd = JobData('myJob',{'arg1':1})
        self.assertEqual('myJob(arg1 = 1)', jd.getCommandLine())

    def testJobWithMultipleArg(self):
        jd = JobData('myJob',{'arg1':1,'arg2':2,'arg3':'three'})
        self.assertEqual("myJob(arg1 = 1, arg2 = 2, arg3 = 'three')",
                         jd.getCommandLine())

class testJobQueueManager(unittest.TestCase ):
    '''
    This class tests the Job Queue Manager.
    '''
    def testInitialization(self):
        pass

        



def suite():
    return [simplecluster_test, testJobData, testJobQueueManager]
 
if __name__ == '__main__':
    testSuite = []
    for testClass in suite():
        testSuite.append(unittest.makeSuite(testClass,'test'))
    allTests = unittest.TestSuite(testSuite)
    unittest.TextTestRunner(verbosity=2).run(allTests)
