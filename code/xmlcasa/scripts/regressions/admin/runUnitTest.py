
""" Script to run unit tests from the command line as: 
    casapy [casa-options] -c runUnitTest.py testname1 testname2...
    casapy [casa-options] -c runUnitTest.py testname1[test_r,test23] testname2...
    casapy [casa-options] -c runUnitTest.py --help
    casapy [casa-options] -c runUnitTest.py --list
    casapy [casa-options] -c runUnitTest.py
    
    or from inside casapy:
    sys.path.append(os.environ["CASAPATH"].split()[0] + '/code/xmlcasa/scripts/regressions/admin')
    import runUnitTest
    runUnitTest.main(['testname']) 
    runUnitTest.main()
    runUnitTest.main(['--short'])
    """

# The main class in testwrapper.py is:
# class UnitTest, methods: getUnitTest(), getFuncTest()
#
# UnitTest.getUnitTest() --> for new scripts as described in ....doc....
# UnitTest.getFuncTest() --> for old tests, which contain functions data() and run()


import os
import sys
import traceback
import unittest
import string
import re
import shutil
sys.path.append(os.environ["CASAPATH"].split()[0] + '/code/xmlcasa/scripts/regressions/admin')
import testwrapper
from testwrapper import *
import nose


# List of tests to run
OLD_TESTS = [
            ]

FULL_LIST = ['test_boxit',
             'test_clean',
             'test_clearstat',
             'test_csvclean',
             'test_exportasdm',
             'test_hanningsmooth',
             'test_imcontsub',
             'test_imfit',
             'test_imhead',
             'test_immath',
             'test_immoments',
             'test_importasdm',
#             'test_importevla',
#             'test_importfitsidi',
             'test_importoldasdm',
             'test_imregrid',
             'test_imsmooth',
             'test_imstat',
             'test_imval',
             'test_listhistory',  
             'test_listvis',           
             'test_plotants',
#             'test_plotms',
             'test_report',
             'test_smoothcal',
             'test_vishead',
             'test_visstat']

SHORT_LIST = ['test_boxit',
             'test_clean',
             'test_csvclean',
             'test_exportasdm',
             'test_imfit',
             'test_imhead',
             'test_importasdm',
             'test_importevla',
             'test_imregrid',
             'test_imstat',
             'test_imval',
             'test_listvis',
             'test_plotants',
             'test_plotms',
             'test_smoothcal',
             'test_vishead']

def usage():
    print '*************************************************************************'
    print '\nRunUnitTest will execute unit test(s) of CASA tasks.'
    print 'Usage:\n'
    print 'casapy [casapy-options] -c runUnitTest.py [options]\n'
    print 'options:'
    print 'no option:        runs all tests defined in FULL_LIST list'
    print '<test_name>:      runs only <test_name> (more tests are separated by spaces)'
    print '--short:          runs only a short list of tests defined in SHORT_LIST'
    print '--file <list>:    runs the tests defined in <list>; one test per line'
    print '--list:           prints the full list of available tests'
    print '--help:           prints this message\n'
    print 'See documentation in: http://www.eso.org/~scastro/ALMA/CASAUnitTests.htm\n'
    print '**************************************************************************'

def list_tests():
    print 'Full list of unit tests'
    print '-----------------------'
    for t in FULL_LIST:
        print t
    
def is_old(name):
    '''Check if the test is old or new'''
    if (OLD_TESTS.__contains__(name)):
        return True
    elif (FULL_LIST.__contains__(name)):
        return False
    else:
        return None

def haslist(name):
    '''Check if specific list of tests have been requested'''
    n0 = name.rfind('[')
    n1 = name.rfind(']')
    if n0 == -1:
        return False
    return True

def getname(testfile):
    '''Get the test name from the command-line
       Ex: from test_clean[test1], returns test_clean'''
    n0 = testfile.rfind('[')
    n1 = testfile.rfind(']')
    if n0 != -1:
        return testfile[:n0]

def gettests(testfile):
    '''Get the list of specific tests from the command-line
       Ex: from test_clean[test1,test3] returns [test1,test3]'''
    n0 = testfile.rfind('[')
    n1 = testfile.rfind(']')
    if n0 != -1:
        temp = testfile[n0+1:n1]
        tests = temp.split(',')
        return tests

def readfile(FILE):
    if(not os.path.exists(FILE)):
        print 'List of tests does not exist'
        return []
    
    List = []
    infile = open(FILE, "r")
    for newline in infile:
        line=newline.rstrip('\n')
        List.append(line)
    
    infile.close()
    return List


# Define which tests to run    
whichtests = 0
        

def main(testnames=[]):

    # Global variable used by regression framework to determine pass/failure status
    global regstate  
    regstate = False
        
    listtests = testnames
    if listtests == '--help':
        usage()
        sys.exit()
    if listtests == '--list':
        list_tests()
        sys.exit()
    if listtests == []:
        whichtests = 0
    elif (listtests == SHORT_LIST or listtests == ['--short']
          or listtests == ['SHORT_LIST']):
        whichtests = 2
        listtests = SHORT_LIST
    elif (type(testnames) != type([])):
        if (os.path.isfile(testnames)):
            # How to prevent it from opening a real test???
            whichtests = 1
            listtests = readfile(testnames)
            if listtests == []:
                raise Exception, 'List of tests is empty'
        else:
            raise Exception, 'List of tests does not exist'
            
    else:
        whichtests = 1
           

    # Directories
    PWD = os.getcwd()
    WDIR = PWD+'/nosedir/'
    
    # Create a working directory
    workdir = WDIR
    print 'Creating working directory '+ workdir
    if os.access(workdir, os.F_OK) is False:
        os.makedirs(workdir)
    else:
        shutil.rmtree(workdir)
        os.makedirs(workdir)
    
    # Move to working dir
    os.chdir(workdir)
    
    # Create a directory for nose's xml files
    xmldir = WDIR+'xml/'
    if os.access(xmldir, os.F_OK) is False:
        os.makedirs(xmldir)
    else:
        shutil.rmtree(xmldir)
        os.makedirs(xmldir)
    
    
    # ASSEMBLE and RUN the TESTS
    '''Run all tests'''
    if not whichtests:
        print "Starting unit tests for %s%s: " %(OLD_TESTS,FULL_LIST)
        
        # Assemble the old tests       
        listtests = OLD_TESTS
        list = []
        for f in listtests:
            try:
                testcase = UnitTest(f).getFuncTest()
                list = list+[testcase]
            except:
                traceback.print_exc()
        
        # Assemble the new tests
        listtests = FULL_LIST        
        for f in listtests:
            try:
                tests = UnitTest(f).getUnitTest()
                list = list+tests
            except:
                traceback.print_exc()
                    
    elif (whichtests == 1):
        '''Run specific tests'''
        # is this an old or a new test?
        oldlist = []
        newlist = []
        for f in listtests:
            if not haslist(f):
                if is_old(f):
                    oldlist.append(f)
                elif is_old(f)==False:
                    newlist.append(f)
                else:
                    print 'WARN: %s is not a valid test name'%f
            else:
                # they are always new tests. check if they are in list anyway
                ff = getname(f)
                if not is_old(ff):
                    newlist.append(f)
                else:
                    print 'Cannot find test '+ff
                
                
        if (len(oldlist) == 0 and len(newlist) == 0):
            os.chdir(PWD)
            raise Exception, 'Tests are not part of any list'
                
        print "Starting unit tests for %s%s: " %(oldlist,newlist)
        
        # Assemble the old tests
        list = []    
        for f in oldlist:
            try:
                testcase = UnitTest(f).getFuncTest()
                list = list+[testcase]
            except:
                traceback.print_exc()
                
        # Assemble the new tests
        for f in newlist:
            try:
                if haslist(f):
                    file = getname(f)
                    tests = gettests(f)
                    testcases = UnitTest(file).getUnitTest(tests)
                    list = list+testcases
                else:
                    testcases = UnitTest(f).getUnitTest()
                    list = list+testcases
            except:
                traceback.print_exc()
                                             
    elif(whichtests == 2):
        '''Run the SHORT_LIST of tests only'''
        print "Starting unit tests for %s: " %SHORT_LIST
        
        # Assemble the short list of tests
        listtests = SHORT_LIST        
        list = []    
        for f in listtests:
            try:
                tests = UnitTest(f).getUnitTest()
                list = list+tests
            except:
                traceback.print_exc()
                
    # Run all tests and create a XML report
    xmlfile = xmldir+'nose.xml'
    try:
        regstate = nose.run(argv=[sys.argv[0],"-d","-s","--with-xunit","--verbosity=2","--xunit-file="+xmlfile],
                              suite=list)
    
        os.chdir(PWD)
    except:
        print "Failed to run one or more tests"
        traceback.print_exc()
    else:
        os.chdir(PWD)


if __name__ == "__main__":
    # Get command line arguments
    
    if "-c" in sys.argv:
        # If called with ... -c runUnitTest.py from the command line,
        # then parse the command line parameters
        i = sys.argv.index("-c")
        if len(sys.argv) >= i + 2 and \
               re.compile("runUnitTest\.py$").search(sys.argv[i + 1]):

            this_file = sys.argv[i+1]

            # Get the tests to run
            testnames = []
    
            la = [] + sys.argv
            
            if len(sys.argv) == i+2:
                # Will run all tests
                whichtests = 0
                testnames = []
            elif len(sys.argv) > i+2:
                # Will run specific tests.
                whichtests = 1
                
                while len(la) > 0:
                    elem = la.pop()
                    if elem == '--help':
                        usage()
                        os._exit(0)
                    if elem == '--list':
                        list_tests()
                        os._exit(0)
                    if elem == '--file':
                        # read list from a text file
                        index = i + 3
                        testnames = sys.argv[index]
                        #                testnames = readfile(filelist)
                        #                if (testnames == []):
                        #                    sys.exit()
                        break
                    if elem == '--short':
                        # run only the SHORT_LIST
                        whichtests = 2
                        testnames = SHORT_LIST
                        break    
                    if elem == this_file:
                        break
                
                    testnames.append(elem)
        else:
            testnames = []
    else:
        # Not called with -c (but possibly execfile() from iPython)
        testnames = []

    try:
        main(testnames)
    except:
        traceback.print_exc()
