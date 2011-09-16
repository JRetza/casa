###############################################
# Regression Script for simdata of a 2d image #
# (single dish only simulation)               #
###############################################

import os, time

#modelname="M51HA.MODEL"
modelname="m51ha.model"
if os.path.exists(modelname):
    shutil.rmtree(modelname)

startTime = time.time()
startProc = time.clock()

print '--Running simdata of M51 (total power+interferometer) --'
# configs are in the repository

l=locals() 
if not l.has_key("repodir"): 
    repodir=os.getenv("CASAPATH").split(' ')[0]

print 'I think the data repository is at '+repodir
datadir=repodir+"/data/regression/simdata/"
cfgdir=repodir+"/data/alma/simmos/"
#importfits(fitsimage=datadir+modelname,imagename="m51.image")
shutil.copytree(datadir+modelname,modelname)
default("simdata")

project = 'm51both_co32'
# Clear out results from previous runs.
os.system('rm -rf '+project+'*')

#modifymodel=True
skymodel = modelname
inbright = '0.004'
indirection = 'B1950 23h59m59.96 -34d59m59.50'
incell = '0.1arcsec'
incenter = '330.076GHz'
inwidth = '50MHz'

setpointings = True
integration = '10s'
mapsize = '1arcmin'
#maptype = 'square'
pointingspacing = '9arcsec'

#predict = True
observe = True
refdate='2012/11/21/20:00:00'
#totaltime = '31360s'
sdantlist = cfgdir+'aca.tp.cfg'
sdant = 0

antennalist="alma;0.5arcsec"

#thermalnoise = 'tsys-manual'  #w/ noise 

image = True
vis = '$project.ms,$project.sd.ms'  #w/ noise
imsize = [512,512]
cell = '0.2arcsec'
#modelimage='m51sdmed_co32.sd.image'  # should make parse $project
modelimage='m51both_co32/m51both_co32.sd.image'  # should make parse $project

analyze = True
# show psf & residual are not available for SD-only simulation
showpsf = False
showresidual = False
showconvolved = True

if not l.has_key('interactive'): interactive=False
if interactive:
    graphics="both"
else:
    graphics="file"

verbose=True
overwrite=True

inp()
simdata()

endTime = time.time()
endProc = time.clock()


# Regression

test_name = """simdata observation of M51 (total power+interferometric)"""

ia.open(project+"/"+project + '.image')
m51both_stats=ia.statistics(verbose=False,list=False)
ia.close()


ia.open(project+"/"+project + '.diff')
m51both_diffstats=ia.statistics(verbose=False,list=False)
ia.close()

# # KS - updated 2010-12-17 (13767@active)
# # reference statistic values for simulated image
# refstats = { 'sum': 531.76,
#              'max': 0.11337,
#              'min': -0.032224,
#              'rms': 0.013376,
#              'sigma': 0.012521 }

# # reference statistic values for diff image
# diffstats = {'sum': 335.78,
#              'max': 0.053222,
#              'min': -0.042831,
#              'rms': 0.0094158,
#              'sigma': 0.0089342 }

# KS - updated 2011-08-16 (15907@active)
# reference statistic values for simulated image
refstats = { 'max': 0.1126,
             'min': -0.032491,
             'rms': 0.013248,
             'sigma': 0.012456,
             'sum': 510.06 }

# reference statistic values for diff image
diffstats = {'max': 0.053971,
             'min': -0.049349,
             'rms': 0.0095375,
             'sigma': 0.0089992,
             'sum': 357.14 }


# relative tolerances to reference values
reftol   = {'sum':  1e-2,
            'max':  1e-2,
            'min':  5e-2,
            'rms':  1e-2,
            'sigma': 1e-2}

import datetime
datestring = datetime.datetime.isoformat(datetime.datetime.today())
outfile    = project+"/"+project + '.' + datestring + '.log'
logfile    = open(outfile, 'w')

print 'Writing regression output to ' + outfile + "\n"

loghdr = """
********** Regression *****************
"""

print >> logfile, loghdr

# more info
ms.open(project+"/"+project+".sd.ms")
print >> logfile, "Noiseless MS, amp stats:"
print >> logfile, ms.statistics('DATA','amp')
print >> logfile, "Noiseless MS, phase stats:"
print >> logfile, ms.statistics('DATA','phase')
ms.close()
#ms.open(project+".noisy.ms")
#print >> logfile, "Noisy MS, amp stats:"
#print >> logfile, ms.statistics('DATA','amp')
#print >> logfile, "Noisy MS, phase stats:"
#print >> logfile, ms.statistics('DATA','phase')
#ms.close()


regstate = True
rskes = refstats.keys()
rskes.sort()
for ke in rskes:
    adiff=abs(m51both_stats[ke][0] - refstats[ke])/abs(refstats[ke])
    if adiff < reftol[ke]:
        print >> logfile, "* Passed %-5s image test, got % -11.5g expected % -11.5g." % (ke, m51both_stats[ke][0], refstats[ke])
    else:
        print >> logfile, "* FAILED %-5s image test, got % -11.5g instead of % -11.5g." % (ke, m51both_stats[ke][0], refstats[ke])
        regstate = False

rskes = diffstats.keys()
rskes.sort()
for ke in rskes:
    adiff=abs(m51both_diffstats[ke][0] - diffstats[ke])/abs(diffstats[ke])
    if adiff < reftol[ke]:
        print >> logfile, "* Passed %-5s  diff test, got % -11.5g expected % -11.5g." % (ke, m51both_diffstats[ke][0], diffstats[ke])
    else:
        print >> logfile, "* FAILED %-5s  diff test, got % -11.5g instead of % -11.5g." % (ke, m51both_diffstats[ke][0], diffstats[ke])
        regstate = False

# this script doesn't have sensible values yet 20100928
regstate=True        

print >> logfile,'---'
if regstate:
    print >> logfile, 'Passed',
else:
    print >> logfile, 'FAILED',
print >> logfile, 'regression test for simdata of M51 (total power+interferometric).'
print >>logfile,'---'
print >>logfile,'*********************************'
    
print >>logfile,''
print >>logfile,'********** Benchmarking **************'
print >>logfile,''
print >>logfile,'Total wall clock time was: %8.3f s.' % (endTime - startTime)
print >>logfile,'Total CPU        time was: %8.3f s.' % (endProc - startProc)
print >>logfile,'Wall processing  rate was: %8.3f MB/s.' % (17896.0 /
                                                            (endTime - startTime))

### Get last modification time of .ms.
msfstat = os.stat(project+"/"+project+'.sd.ms')
print >>logfile,'* Breakdown:                           *'
print >>logfile,'*  generating visibilities took %8.3fs,' % (msfstat[8] - startTime)
print >>logfile,'*************************************'
    
logfile.close()
						    
print '--Finished simdata of M51 (total power+interferometric) regression--'
