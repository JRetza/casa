########################################################################3
#  imsmooth_test.py
#
#
# Copyright (C) 2008, 2009
# Associated Universities, Inc. Washington DC, USA.
#
# This scripts free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
# Correspondence concerning AIPS++ should be adressed as follows:
#        Internet email: aips2-request@nrao.edu.
#        Postal address: AIPS++ Project Office
#                        National Radio Astronomy Observatory
#                        520 Edgemont Road
#                        Charlottesville, VA 22903-2475 USA
#
# <author>
# Shannon Jaeger (University of Calgary)
# </author>
#
# <summary>
# Test suite for the CASA imsmooth Task
# </summary>
#
# <reviewed reviwer="" date="" tests="" demos="">
# </reviewed
#
# <prerequisite>
# <ul>
#   <li> <linkto class="imsmooth.py:description">imsmooth</linkto> 
#   <li> <linkto class="imregion.py:description">imregion</linkto> 
# </ul>
# </prerequisite>
#
# <etymology>
# imsmooth_test stands for image momemnts test
# </etymology>
#
# <synopsis>
# imsmooth_test.py is a Python script that tests the correctness
# of the imsmooth task in CASA.
#
# The tests include:
#    1. Incorrect input for each paramter.  Incorrect input includes
#       one input of the incorrect data type, out-of-bounds (where
#       applicable, and correct data type but non-sensical.
#    2. Do smoothing on a point source, since the results are well-defined
#       and we can check for it.  Also do a smoothing on an image.
#    3. Smoothing with region selection on the sky, channels
#       and stokes values, as well as using an input region file
#
# In the imsmooth task the data points in the image are smoothed
# or normalized, the points used in the normalization of a pixel
# are based on the kernel shape selected.
#
# The expected input is an image, and a new image is created
# from it.
#
# By default the smoothing is applied to the whole image, however,
# if a region file is given and/or the user specifies a box range,
# channels, or stokes values then the subtraction is performed
# on this portion of the image only.
#
# </synopsis> 
#
# <example>
# # This test was designed to run in the automated CASA test system.
# # This example shows who to run it manually from with casapy.
# import os
# sys.path.append( os.environ["CASAPATH"].split()[0]+'/code/xmlcasa/scripts/regressions/admin' )
# import publish_summary
# publish_summary.runTest( 'imsmooth_test' )
#
# To test the script by itself, ie. not with test infrastructure.
# where WORKING_DIR is where you'll run the script
#       CASA_REPOS is where casa is installed
#
# cp CASA_REPOS/data/regression/g192redux/reference/* WORKING_DIR
# cp CASA_REPOS/code/xmmlcasa/tasts/tests/imsmooth_test.py WORKING_DIR
# casapy
# CASA <2>: cd WORKING_DIR
# CASA <3>: import imsmooth_test
# CASA <4>: imsmooth_test.run()
# </example>
#
# <motivation>
# To provide a test standard to the imsmooth task to try and ensure
# coding changes do not break the 
# </motivation>
#
# <todo>
#
# 
# </todo>

import random
import os
import numpy
import shutil
import casac
from tasks import *
from taskinit import *

    
####################################################################
# Incorrect inputs to parameters.  The parameters are:
#    imagename
#    outfile
#    kernel
#    major
#    minor
#    mask
#    region
#    box
#    chans
#    stokes
#
# Returns True if successful, and False if it has failed.
####################################################################
def input_test():
    retValue = {'success': True, 'msgs': "", 'error_msgs': '' }
    casalog.post( "Starting imsmooth INPUT/OUTPUT tests.", 'NORMAL2' )

    # First step get rid of all the old test files!
    for file in os.listdir( '.' ):
        if file.startswith( 'input_test' ):
            shutil.rmtree( file )
    if os.path.exists( 'garbage.rgn' ):
        os.remove('garbage.rgn')


    #######################################################################
    # Testing the imagename parameter.
    #    1. Bad file name should throw and exception
    #    2. Good file name, a file should be
    #######################################################################
    casalog.post( "The IMAGENAME parameter tests will cause errors to occur, do not be alarmed", 'WARN' )
    
    try:
        result = None
        results = imsmooth( 'g192', outfile='input_test1' )
    except:
        no_op='noop'
    else:
        if ( results != None and results!=False):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Badfile, 'g192', was not reported as missing."
        
    
    try:
        results = None
        results = imsmooth( 'g192_a2.image', outfile='input_test1' )

    except:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                +"\nError: Unable to smooth g192_a2.image"
        
    if (  not os.path.exists( 'input_test1' ) and results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                +"\nError: continuum files for 'input_test1' were not created."


    #######################################################################
    # Testing the outfile parameter.
    #    1. Bad file, file already exists, exception should be thrown
    #    2. Good file name, a file should be
    #######################################################################
    casalog.post( "The OUTFILE parameter tests will cause errors to occur, do not be alarmed", 'WARN' )
    
    results = None
    results = imsmooth( 'g192_a2.image', outfile='input_test1' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Badfile, 'input_test1', was not reported as already existing."
        
    
    try:
        results = None
        results=imsmooth( 'g192_a2.image', outfile='input_test2' )
    except:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Unable to create smoothed image 'input_test2'"
    if ( not os.path.exists( 'input_test2' ) and results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: output file, 'input_test2', was not created."


    #######################################################################
    # Testing KERNEL parameter, valid values 0 and greater
    #    1. Below invalid range: junk, ''
    #    3. valid: gaussian, boxcar, tophat, 
    #######################################################################
    casalog.post( "The KERNEL parameter tests will cause errors to occur, do not be alarmed", 'WARN' )
    
    results = None
    try:
        results = imsmooth( 'g192_a2.image', kernel='', outfile='input_test3' )
    except:
        no_op='noop'
    else:
        if ( results != None ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: No exception thrown for bad kernel value, ''"

    results = None
    try:
        results = imsmooth( 'g192_a2.image', kernel='junk', outfile='input_test4' )
    except:
        no_op='noop'
    else:
        if ( results != None ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: No exception thrown for bad kernel value, 'junk'"    

    results = None
    try:
        results=imsmooth( 'g192_a2.image', kernel='gauss', outfile='input_test5' )
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Gaussian smoothing failed."
    if ( not os.path.exists( 'input_test5' ) or results == None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: input_test5 output file was NOT created."\
                   + "\n\tRESULTS: "+str(results)
        

    results = None
    try:
        results=imsmooth( 'g192_a2.image', kernel='boxcar', outfile='input_test6' )
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Boxcar smoothing failed. " \
                 +str(err)
        
    if ( not os.path.exists( 'input_test6' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: output file 'input_test6' was NOT created."


    # UNCOMMENT when tophat support has been added.
    #try:
    #    results = None
    #    results=imsmooth( 'g192_a2.image', kernel='tophat', oufile='input_test7' )
    #except Exception, err:
    #    retValue['success']=False
    #    retValue['error_msgs']=retValue['error_msgs']\
    #             +"\nError: Tophat smoothing failed. "
    #    
    #if ( not os.path.exists( 'input_test7' ) or results==None ): 
    #    retValue['success']=False
    #    retValue['error_msgs']=retValue['error_msgs']\
    #               +"\nError: output file 'input_test7' was NOT created."



    #######################################################################
    # Testing MAJOR parameter
    # Expects a numerical value: 1, '2pix', '0.5arcsec'
    # Tests include: invalid values, valid values, major < minor 
    #######################################################################
    casalog.post( "The MAJOR parameter tests will cause errors to occur, do not be alarmed", 'WARN' )
    
    try:
        result = None
        results = imsmooth( 'g192_a2.image', major='bad', oufile='input_test8')
    except:
        no_op='noop'
    else:
        if ( results != None ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Bad major value, 'bad', was not reported."

    try:
        result = None
        results = imsmooth( 'g192_a2.image', major=-5, oufile='input_test9' )
    except:
        no_op='noop'
    else:
        if ( results != None ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Bad major value, '-5', was not reported."

    result = None
    try:
        results = imsmooth( 'g192_a2.image', major=2, minor=1, outfile='input_test11')
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Unable to smooth with major=2 and minor=1 "

    if ( not os.path.exists( 'input_test11' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Smoothing failed with numerical major/minor values."\
                   +"\nRESULTS: "+str(results)
        
    result = None
    try:
        results = imsmooth( 'g192_a2.image', major='2pix', minor='1pix', outfile='input_test12')
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Unable to smooth with major='2pix' and minor='1pix' "

    if ( not os.path.exists( 'input_test12' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Smoothing failed with pixel major/minor values."

    result = None
    try:
        results = imsmooth( 'g192_a2.image', major='1.5arcsec', minor='1arcsec', outfile='input_test13')
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Unable to smooth with major='1.5arcsec' and minor='1arcsec' "

    if ( not os.path.exists( 'input_test13' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Smoothing failed with arcsecond major/minor values."
    result = None
    try:
        results = imsmooth( 'g192_a2.image', major='0.5arcsec', minor='2arcsec', outfile='input_test14')
    except:
        no_op='noop'
    else:
        if ( results != None ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Bad major value less than minor value was not reported."        


    #######################################################################
    # Testing REGION parameter
    # Expects a file containing a region record, as created by the viewer.
    # Tests include bad file name, file with bad content, and good file.
    ####################################################################### 
    casalog.post( "The REGION parameter tests will cause errors to occur, do not be alarmed", 'WARN' )

    results = imsmooth( 'g192_a2.image', region=7 )
    if ( results ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad region file, 7, was not reported as bad."



    try:
        results = imsmooth( 'g192_a2.image', region='garbage.rgn' )
    except:
        #We want this to fail
        no_op = 'noop'
    else:
        if ( results ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                                    +"\nError: Bad region file, 'garbage.rgn', was not reported as missing."

    try:
        filename = os.getcwd()+'/garbage.rgn'
        fp=open( filename, 'w' )
        fp.writelines('This file does NOT contain a valid CASA region specification\n')
        fp.close()

        try:
            results = imsmooth( 'g192_a2.image', region=filename )
        except:
            no_op='noop'
        else:
            if ( results ):
                retValue['success']=False
                retValue['error_msgs']=retValue['error_msgs']\
                          + "\nError: Bad region file, 'garbage.rgn',"\
                          + " was not reported as bad."
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Unable to create bad region file.\n\t"
        raise Exception, err

    
    try:
        results=imsmooth( 'g192_a2.image', region='g192_a2.image-2.rgn', outfile='input_test15' )
    except:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Unable to do smoothing with region file g192_a2.image-2.rgn"
    if ( not os.path.exists( 'input_test15' ) or not results ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: output file 'input_test15', was not created."



    #######################################################################
    # Testing BOX parameter
    # The input file has pixel values ranging from
    #   0-511, 0-511
    # Tests include -3, -1, 0, 1 random valid value, 500, 511, 525
    #   for both the x, and y coords
    #
    # Note: -1 is a special case implying use the full range, so to
    #       be out of bounds we need -2 or less.
    #######################################################################
    casalog.post( "The BOX parameter tests will cause errors to occur, do not be alarmed", 'WARN' )
    
    results = None
    results = imsmooth( 'g192_a2.image', box='-3,0,511,511' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad box value, 'x=-3', was not reported as bad."

    results = None    
    results = imsmooth( 'g192_a2.image', box='0,-3,511,511' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad box value, 'y=-3', was not reported as bad."
    
    results = None
    results = imsmooth( 'g192_a2.image', box='-2,0,511,511' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad box value, 'x=-2', was not reported."\
             +"\n\tRESULTS: "+str(results)

    results = None
    results = imsmooth( 'g192_a2.image', box='0,-2,511,511' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad box value, 'y=-2', was not reported as bad."

    results = None
    results = imsmooth( 'g192_a2.image', box='0,0,512,511' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad box value, 'x=512', was not reported as bad."

    results = None    
    results = imsmooth( 'g192_a2.image', box='0,0,511,512' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad box value, 'y=512', was not reported as bad."

    results = None
    results = imsmooth( 'g192_a2.image', box='0, 0,525,511' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad box value, 'x=525', was not reported as bad."

    results = None
    results = imsmooth( 'g192_a2.image', box='0,0,511,525' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad box value, 'y=525', was not reported as bad."

    x1=random.randint(0,511)
    x2=random.randint(x1,511)
    y1=random.randint(0,511)
    y2=random.randint(y1,511)
    boxstr=str(x1)+','+str(y1)+','+str(x2)+','+str(y2)
    
    try:
        results = None
        results = imsmooth( 'g192_a2.image', box=boxstr, outfile='input_test16' )
    except:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Unable to smooth in box="+boxstr
    if ( not os.path.exists( 'input_test16' ) or results==None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: output file 'input_test_box16' was not "\
                 +"created at "+boxstr


    #######################################################################
    # Testing CHANS parameter: valid values 0-39 for our image
    # Values used for testing, -5,-2,0,22~35, 39,40,45
    #
    # NOTE: a coord value of -1 indicates use all, so -1 is a valid
    #       coordiante.
    #######################################################################
    casalog.post( "The CHANS parameter tests will cause errors to occur, do not be alarmed", 'WARN' )
    
    results = None    
    results = imsmooth( 'g192_a2.image', chans='-5' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad channel value, '-5', was not reported."

    results = None    
    results = imsmooth( 'g192_a2.image', chans='-2' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad channel value, '-2', was not reported."\
             +"\n\tRESULTS: "+str(results)

    results = None        
    results = imsmooth( 'g192_a2.image', chans='-18' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad channel value of -18 was not reported."

    results = None        
    results = imsmooth( 'g192_a2.image', chans='45' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad channel value of 45 was not reported."

    results = None        
    results = imsmooth( 'g192_a2.image', chans='40' )
    if ( results != None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
             +"\nError: Bad channel value of 40 was not reported."

    
    results = None            
    try:
        results = imsmooth( 'g192_a2.image', chans='22~35', outfile='input_test17' )
    except:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Unable to smooth on chans=22~35 only "
    if ( not os.path.exists( 'input_test17' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: output file, 'input_test17', was not created."

    results = None
    try:
        results = imsmooth( 'g192_a2.image', chans='0', outfile='input_test17b' )
    except:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Unable to create smooth chans=0 only"
    if ( not os.path.exists( 'input_test17b' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: output file 'input_test17b`' was not created."\
                 +"\nRESULT: "+str(results)

    results=None
    try:
        results = imsmooth( 'g192_a2.image', chans='39', outfile='input_test18' )
    except:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Unable to smooth with chans=39 only"
    if ( not os.path.exists( 'input_test18' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: output file 'input_test18' was not created."

        
    #######################################################################
    # Testing STOKES parameter, valid values: 'I'
    #    Tests are 'Q', 'yellow' (invalid) and 'I'
    #######################################################################
    casalog.post( "The STOKES parameter tests will cause errors to occur, do not be alarmed", 'WARN' )
    
    results=None
    try:
        results = imsmooth( 'g192_a2.image', stokes='Q' )
    except:
        no_op='noop'
    else:
        if ( results!=None ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Bad stokes value, 'Q', was not reported."
            
    results=None    
    try:
        results = imsmooth( 'g192_a2.image', stokes='yellow' )
    except:
        no_op='noop'
    else:
        if ( results!=None ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Bad stokes value, 'yellow', was not reported."

    results = None
    try:
        results = imsmooth( 'g192_a2.image', stokes='I', outfile='input_test19' )
    except:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Smmoothing failed with stokes=Q"
    if ( not os.path.exists( 'input_test19' ) or results==None ):
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: output file 'input_test19' was not created."

    #print "RETURNING", retValue
    return retValue


####################################################################
# Smoothing correctness test.
#
# This test subtacts the continuum from the g192 data file
# and compares the results (both continuum and spectral line
# with subtracted continuum files) with pervious results.
#
# Random values are selected in the files and compared.
#
# Returns True if successful, and False if it has failed.
####################################################################
 
def smooth_test():
    retValue = {'success': True, 'msgs': "", 'error_msgs': '' }
    casalog.post( "Starting imsmooth CORRECTNESS tests.", 'NORMAL2' )

    # First step get rid of all the old test files!
    for file in os.listdir( '.' ):
        if file.startswith( 'smooth_test' ):
            shutil.rmtree( file )

    if os.path.exists( 'smooth.pointsrc.image' ):
        shutil.rmtree( 'smooth.pointsrc.image' )

    # Second step is to create a file with a single point
    # source so that we can check the correctness.  The
    # resulting convolution should be the same shape as
    # the kernel that is used if done correctly.  Also the
    # area under the kernel should be equivalent to the value
    # our point source.
    #
    # We use the the coordinate system from the g192 image
    # and make our image the same size.  In theory it could
    # be anything, it's nice having a coordinate system for
    # the image.
    try:
        # Get the coordinate system and size of the image
        ia.open( 'g192_a2.image' )
        csys = ia.coordsys()
        bb = ia.boundingbox()
        shape = bb['bbShape']
        ia.done()

        # Create an array of zero's, then set position 212,220,0,20
        # to 100 (our point source).
        #
        # Note that 
        inputArray = numpy.zeros( (shape[0], shape[1], shape[2], shape[3]), 'float' )
        inputArray[212,220,0,20] = 100

        # Now make the image!
        ia.fromarray( pixels=inputArray, csys=csys.torecord(), \
                      outfile='smooth.pointsrc.image' )
        ia.done()
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Unable to create point source image."\
                 +"\n\t REULTS: "+str(err)        

    # Do a Gaussian smoothing with major axis of 50, and minor of 25
    # pixels.  We expect the resulting image to have a Gussian shape,
    # with    max at:    212,220,0,20
    #         1st sd:    from
    #         2nd sd:    from

    results = None
    try:
        results=imsmooth( 'smooth.pointsrc.image', kernel='gauss', \
                          major=50, minor=25, outfile='smooth_test1' )
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: boxcar smooth failed on point source file."

        
    if ( not os.path.exists( 'smooth_test1' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Gaussian smoothfailed on point source file."
    else:
        # Now that we know something has been done lets check the results!
        #      1. Check that the sum of the values under the curve is 100
        #      2. Check that the max is at 212, 220, 0 , 20
        allowedError = 0.009
        
        ia.open( 'smooth_test1')
        stats = ia.statistics()
        sum = stats['sum'][0]
        if ( ( sum < 100 and sum < ( 100-allowedError ) )
             or ( sum > 100 and sum > ( 100+allowedError) ) ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                +"\nError: Sum under Gaussian is "+str(stats['sum'][0])\
                +" expected 100."

        maxpos=stats['maxpos'].tolist()
        if ( maxpos[0]!=212 or maxpos[1]!=220 or \
             maxpos[2]!=0 or maxpos[3]!=20 ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                +"\nError: Max position found at "+str(maxpos)\
                +" expected it to be at 212,220,0,20."            
        
        ia.done()
            

    # Do a box car smooth and verify expected results as follows:
    #
    results = None
    try:
        results=imsmooth( 'smooth.pointsrc.image', kernel='boxcar', \
                          major=20, minor=10, outfile='smooth_test2' )
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: boxcar smooth failed on point source file."
        
    if ( not os.path.exists( 'smooth_test2' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: output file 'smooth_test2' was NOT created."
    else:
        # Now that we know something has been done lets check the results!
        #        1. Check that the sum of the points is 100
        #        2. That the points in the box are 0.125=(100/((10+10)*(20+20))
        ia.open( 'smooth_test2')
        stats = ia.statistics()
        if ( ( sum < 100 and sum < ( 100-allowedError ) )
             or ( sum > 100 and sum > ( 100+allowedError) ) ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                +"\nError: Sum under Gaussian is "+str(stats['sum'][0])\
                +" expected 100."

        val1 = ia.pixelvalue( [ 204,200,0,20] )
        val2 = ia.pixelvalue( [ 222,236,0,20] )
        val3 = ia.pixelvalue( [ 204,239,0,20] )
        val4 = ia.pixelvalue( [ 222,201,0,20] )        
        midVal = ia.pixelvalue( [212,220,0,20] )
        for value in [val1, val2, val3, val4, midVal ]:
            if ( value>(0.125-allowedError) and value<(0.125+allowedError)):
                retValue['success']=False
                retValue['error_msgs']=retValue['error_msgs']\
                    +"\nError: Values in the smoothed box are not all 0.125"\
                    +" found value of "+str(value)
        ia.done()

    return retValue


####################################################################
# Region selection correction test.
#
# This test selects a region(s) where smoothing will be performed.
#
# Returns True if successful, and False if it has failed.
####################################################################

def region_test():
    retValue = {'success': True, 'msgs': "", 'error_msgs': '' }
    casalog.post( "Starting imsmooth REGION tests.", 'NORMAL2' )
    allowedError = 0.0005

    # First step get rid of all the old test files!
    for file in os.listdir( '.' ):
        if file.startswith( 'rgn_test' ):
            shutil.rmtree( file )

    if os.path.exists( 'rgn.pointsrc.image' ):
        shutil.rmtree( 'rgn.pointsrc.image' )

    # Second step is to create a file with a single point
    # source so that we can check the correctness.  The
    # resulting convolution should be the same shape as
    # the kernel that is used if done correctly.  Also the
    # area under the kernel should be equivalent to the value
    # our point source.
    #
    # We use the the coordinate system from the g192 image
    # and make our image the same size.  In theory it could
    # be anything, it's nice having a coordinate system for
    # the image.
    try:
        # Get the coordinate system and size of the image
        ia.open( 'g192_a2.image' )
        csys = ia.coordsys()
        bb = ia.boundingbox()
        shape = bb['bbShape']
        ia.done()

        # Create an array of zero's, then set a couple positions (point
        # sources) to 100.
        #
        # Note that 
        inputArray = numpy.zeros( (shape[0], shape[1], shape[2], shape[3]), 'float' )

        inputArray[49,71,0,14] = 100     # For rgn file
        inputArray[233,276,0,20] = 100     # For rgn in image

        
        # Now make the image!
        ia.fromarray( pixels=inputArray, csys=csys.torecord(), \
                      outfile='rgn.pointsrc.image' )
        ia.done()
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Unable to create point source image."\
                 +"\n\t REULTS: "+str(err)        


    # Select the following regions without the point source:
    #            1. Sky region without the point source
    #            2. Channel that doesn't have the point source
    #
    # Note: that when we check the resulting smoothed images
    #       the should be empty.

    results = None
    try:
        results=imsmooth( 'rgn.pointsrc.image', kernel='gauss', \
                          major=50, minor=25, outfile='rgn_test1', \
                          box='350,350,375,390')
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Smoothng failed on region 250,250,275,290."

        
    if ( not os.path.exists( 'rgn_test1' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Smoothing failed on region 250,250,275,290."
    else:
        # Now that we know something has been done lets check the results!
        #      1. Check that the sum of the values under the curve is 0
        ia.open( 'rgn_test1')
        stats = ia.statistics()
        if ( stats['sum'][0] < ( 0-allowedError) \
             or stats['sum'][0] > ( 0+allowedError) ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                +"\nError: Sum on smoothed file rgn_test1 is "\
                +str(stats['sum'][0]) +" expected value is 0."
        ia.done()


    results = None
    try:
        results=imsmooth( 'rgn.pointsrc.image', kernel='gauss', \
                          major=50, minor=25, outfile='rgn_test2', \
                          chans='22')
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Smoothng failed on channel 22."

        
    if ( not os.path.exists( 'rgn_test2' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Smoothing failed on channel 22."
    else:
        # Now that we know something has been done lets check the results!
        #     1. Check that the sum of the values under the curve is 0
        ia.open( 'rgn_test2')
        stats = ia.statistics()
        if ( stats['sum'][0] < ( 0-allowedError) \
             or stats['sum'][0] > ( 0+allowedError) ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                +"\nError: Sum on smoothed file rgn_test2 is "\
                +str(stats['sum'][0]) +" expected value is 0."
        ia.done()



    # Select a region that contains the point source
    #   1. using imsmooth parameters
    #   2. region defined in an image
    #        g192_a.image:testregion (blc=166,222,0,0  trc=296,328,0,39)
    #   3. region file.
    #        g192_1.image.rgn      (blc=0,0,0,0 trc=511,511,0,14)
    #
    results = None
    try:
        results=imsmooth( 'rgn.pointsrc.image', kernel='gauss', \
                          major=10, minor=5, outfile='rgn_test3', \
                          chans='14', box='0,0,200,200')
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Smoothng failed on channel 14, box 0,0,200,200."

        
    if ( not os.path.exists( 'rgn_test3' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Smoothing failed on channel 14, box 0,0,200,200."
    else:
        # Now that we know something has been done lets check the results!
        #     1. Check that the sum of the values under the curve is 100
        #     2. Check that the max is at 49,71, 0, 14
        ia.open( 'rgn_test3')
        stats = ia.statistics()
        if ( stats['sum'][0] < ( 100-allowedError) \
             or stats['sum'][0] > ( 100+allowedError) ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                +"\nError: Sum on smoothed file rgn_test3 is "\
                +str(stats['sum'][0]) +" expected value is 100."
        ia.done()

        # Note that since we've selected a single plane then our
        # output image has a single plane, 0, only!  Thus, unlike
        # our original image the max point should be found on the
        # 0th channel and NOT the 14th channel.
        maxpos=stats['maxpos'].tolist()
        if ( maxpos[0]!=49 or maxpos[1]!=71 or \
             maxpos[2]!=0 or maxpos[3]!=0 ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                +"\nError: Max position found at "+str(maxpos)\
                +" expected it to be at 49,71,0,0."            


    results = None
    try:
        results=imsmooth( 'rgn.pointsrc.image', kernel='gauss', \
                          major=12, minor=7, outfile='rgn_test5', \
                          region='g192_a2.image:testregion')
    except Exception, err:
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                 +"\nError: Smoothng failed with internal image region 'testregion'."

        
    if ( not os.path.exists( 'rgn_test5' ) or results==None ): 
        retValue['success']=False
        retValue['error_msgs']=retValue['error_msgs']\
                   +"\nError: Smoothing failed internal image region 'testregion'."
    else:
        # Now that we know something has been done lets check the results!
        #     1. Check that the sum of the values under the curve is 100
        #     2. Check that the max is at 49,71, 0, 14
        ia.open( 'rgn_test5')
        stats = ia.statistics()
        ia.done()
        
        sum = stats['sum'][0]
        allowedError=0.00075
        if ( ( sum < 100 and sum < ( 100-allowedError ) )
             or ( sum > 100 and sum > ( 100+allowedError) ) ):
             
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                +"\nError: Sum on smoothed file rgn_test4 is "\
                +str(stats['sum'][0]) +" expected value is 100."

        # Max position = point src position - minx, miny of region    
        maxpos=stats['maxpos'].tolist()
        if ( maxpos[0]!=233-166 or maxpos[1]!=276-222 or \
             maxpos[2]!=0 or maxpos[3]!=20 ):
            retValue['success']=False
            retValue['error_msgs']=retValue['error_msgs']\
                +"\nError: Max position found at "+str(maxpos)\
                +" expected it to be at 212,220,0,20."            

    return retValue



####################################################################
# Methods for the automated CASA testing
####################################################################

def description():
    return " Tests the correctness of the imsmooth task in CASA including:"\
           + "\n\n"\
           + "1. Incorrect input for each paramter.  Incorrect input includes"\
           + "   one input of the incorrect data type, out-of-bounds (where"\
           + "   applicable, and correct data type but non-sensical."\
           + "2. Do some smoothings on a point source to verify results "\
           + ".  as well as on an image. "\
           + "3. Doing smoothing subtrating with region selection on the sky,"\
           + "   channels, and stokes values, as well as using an input"\
           + "   region file."\
           + "\n\n"\
           + "Data used for this test includes: "\
           + "     1. g192_a2.image"

def data():
    return ['g192_a2.image', 'g192_a2.image-2.rgn', 'g192_a2.image.rgn' ]


def doCopy():
    #print "\n\nIn IMSMOOTH doCopy()\n\n"
    return [1, 1, 1 ]

def run():
    test_list = [ 'input_test()', 'smooth_test()', \
                  'region_test()' ]


    # This would be really, really, really, really nice to run in a loop
    # and use the eval() command to execute the various methods BUT for
    # some idiotic reason, which I do not know, when you do this. The
    # casapy function, update_params() can not be found and you get an
    # error msg but if you call each method directly then there is no issue.
    
    passed = True
    error_msgs = ""
    testResults=[]
    testResults.append( input_test() )
    testResults.append( smooth_test() )
    testResults.append( region_test() )
    print "TEST RESULTS: ", testResults

    for results in testResults:
        #print "RESULTS: ", results
        #print "ERRORS: ", results['error_msgs']
        if ( not results['success'] ):
            passed = False
            error_msgs = error_msgs + "\n" + results['error_msgs']

    print "ERROR MES: ", error_msgs
    print "PASSED: ", passed
    if ( not passed ):
        casalog.post( error_msgs, 'EXCEPTION' )
        raise Exception, 'imval test has failed!\n'+error_msgs
    
    return []
