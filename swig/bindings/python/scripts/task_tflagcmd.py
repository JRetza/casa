from taskinit import *
import time
import os
import sys
import flaghelper as fh

debug = False

def tflagcmd(
    vis=None,
    inpmode=None,
    inpfile=None,
    tablerows=None,
    useapplied=None,
    reason=None,
    command=None,
    tbuff=None,
    ants=None,
    action=None,
    flagbackup=None,
    clearall=None,
    rowlist=None,
    plotfile=None,
    display=None,
    writeflags=None,
    sequential=None,
    savepars=None,
    outfile=None,
    async=None
    ):
    #
    # Task tflagcmd
    #    Reads flag commands from file or string and applies to MS

    # TO DO:
    # remove sorting
    # add reason selection in all input types
    # what to do with the TIME column?
    # implement backup for the whole input file
    # add support for ASDM.xml ????


    try:
        from xml.dom import minidom
    except:
        raise Exception, 'Failed to load xml.dom.minidom into python'

    casalog.origin('tflagcmd')
#    casalog.post('You are using flagcmd v3.6 Updated STM 2011-06-28')

    tflocal = casac.homefinder.find_home_by_name('testflaggerHome').create()
    mslocal = casac.homefinder.find_home_by_name('msHome').create()

    # MS HISTORY
    mslocal.open(vis, nomodify=False)
    mslocal.writehistory(message='taskname = tflagcmd', origin='tflagcmd')
    mslocal.open(vis, nomodify=False)

    try:
        # Use a default ntime to open the MS. The user-set ntime will be
        # used in the tool later
        ntime = 0.0
        
        # Open the MS and attach it to the tool
        if ((type(vis) == str) & (os.path.exists(vis))):
            tflocal.open(vis, ntime)
        else:
            raise Exception, 'Visibility data set not found - please verify the name'



        # Get overall MS time range for later use (if needed)
#        try:
            # this might take too long for large MS
        mslocal.open(vis)
        timd = mslocal.range(['time'])
        mslocal.close()
        
        ms_startmjds = timd['time'][0]
        ms_endmjds = timd['time'][1]
        t = qa.quantity(ms_startmjds, 's')
        t1sdata = t['value']
        ms_starttime = qa.time(t, form='ymd', prec=9)
        ms_startdate = qa.time(t, form=['ymd', 'no_time'])
        t0 = qa.totime(ms_startdate + '/00:00:00.00')
        t0d = qa.convert(t0, 'd')
        t0s = qa.convert(t0, 's')
        t = qa.quantity(ms_endmjds, 's')
        t2sdata = t['value']
        ms_endtime = qa.time(t, form='ymd', prec=9)
        # NOTE: could also use values from OBSERVATION table col TIME_RANGE
        casalog.post('MS spans timerange ' + ms_starttime + ' to '
                     + ms_endtime)
            
        
        
        myflagcmd = {}
        cmdlist = []
        unionpars = {}
        if action == 'clear':
            casalog.post('Action "clear" will disregard inpmode (no reading)')
            
        elif inpmode == 'table':
            casalog.post('Reading from FLAG_CMD table')
            # Read from FLAG_CMD table into command list
            if inpfile == '':
                msfile = vis
            else:
                msfile = inpfile

            # Read only the selected rows for action = apply and 
            # always read all rows with APPLIED=True for action = unapply
            if action == 'apply' or action == 'list':
                myflagcmd = readFromTable(msfile, myflagrows=tablerows, useapplied=useapplied,
                                          myreason=reason)
            elif action == 'unapply':
                myflagcmd = readFromTable(msfile, useapplied=True, myreason=reason)
                # Check if selected rows are in read dictionary               
            else:
                myflagcmd = readFromTable(msfile, useapplied=useapplied, myreason=reason)
                                    
            
            listmode = 'cmd'
            
        elif inpmode == 'file':

            casalog.post('Reading from ASCII file')
            ###### TO DO: take time ranges calculation into account ??????
            # Parse the input file
            try:
                if inpfile == '':
                     casalog.post('Input file is empty', 'ERROR')
                     
                cmdlist = fh.readFile(inpfile)
                # Make a FLAG_CMD compatible dictionary
                myflagcmd = fh.makeDict(cmdlist)
                
                # Number of commands in dictionary
                vrows = myflagcmd.keys()

            except:
                raise Exception, 'Error reading the input file '+inpfile
            
            casalog.post('Read ' + str(vrows.__len__())
                         + ' lines from file ' + inpfile)

            # Read ASCII file into command list
#            if inpfile == '':
#                casalog.post('Input file is empty', 'ERROR')
#            
#            flagtable = inpfile
#
#            if (type(flagtable) == str) & os.path.exists(flagtable):
#                try:
#                    ff = open(flagtable, 'r')
#                except:
#                    raise Exception, 'Error opening file ' + flagtable
#            else:
#                raise Exception, \
#                    'ASCII file not found - please verify the name'
#                    
#            #
#            # Parse file
#            try:
#                cmdlist = ff.readlines()
#                 # First remove any blank lines
#                blanks = cmdlist.count('\n')
#                for i in range(blanks):
#                    cmdlist.remove('\n')
#
#            except:
#                raise Exception, 'Error reading lines from file ' \
#                    + flagtable
#            ff.close()
#            casalog.post('Read ' + str(cmdlist.__len__())
#                         + ' lines from file ' + flagtable)
#
#            if cmdlist.__len__() > 0:
#                myflagcmd = readFromFile(cmdlist, ms_startmjds,ms_endmjds,myreason=reason)                               

            listmode = 'file'
            
        elif inpmode == 'xml':

            casalog.post('Reading from Flag.xml')
            # Read from Flag.xml (also needs Antenna.xml)
            if inpfile == '':
                flagtable = vis
            else:
                flagtable = inpfile
                
            # Actually parse table
            myflags = readFromXML(flagtable, mytbuff=tbuff)
            casalog.post('%s'%myflags, 'DEBUG')

            # Construct flags per antenna, selecting by reason if desired
            # Do not resort at this time
            if ants != '' or reason != '':
                myflagcmd = sortflags(myflags, myantenna=ants,
                        myreason=reason, myflagsort='')
            else:
                myflagcmd = myflags

            listmode = 'online'
            
        else:

            # command strings
            cmdlist = command
            casalog.post('Input ' + str(cmdlist.__len__())
                         + ' lines from input list')

            if cmdlist.__len__() > 0:
                myflagcmd = readFromCmd(cmdlist, ms_startmjds, ms_endmjds)

            listmode = ''


        # Specific for some actions
        # Get the commands as a list of strings with reason back in
#        if action == 'apply' or action == 'unapply'  or action == 'save':               
        if action == 'apply' or action == 'unapply'  or action == 'list':               
            # If there are no flag commands, exit
            if myflagcmd.__len__() == 0:
                casalog.post("There are no flag commands in input","WARN")
                return
            
            # Turn into command string list (add reason back in)
            mycmdlist = []
            keylist = myflagcmd.keys()
            if keylist.__len__() > 0:
                for key in keylist:
                    cmdstr = myflagcmd[key]['command']
                    if myflagcmd[key]['reason'] != '':
                        cmdstr += " reason='" + myflagcmd[key]['reason'] \
                            + "'"
                    mycmdlist.append(cmdstr)
                    
            casalog.post('Extracted ' + str(mycmdlist.__len__())
                         + ' flag commands', 'DEBUG')

            casalog.post('%s'%mycmdlist,'DEBUG')
            casalog.post('%s'%myflagcmd,'DEBUG')
            
        #
        # ACTION to perform on input file
        #
        casalog.post('Executing action = ' + action)

        # TODO: check row selection
        # List the flag commands from inpfile on the screen 
        # and save them or not to outfile
        if (action == 'list'):
            
            # List the flag cmds on the screen
            listFlagCmd(myflagcmd, myantenna=ants, myreason=reason, myoutfile='', 
                            listmode=listmode)
            
            
            # Save the flag cmds to the outfile
            if savepars:
                # Save on the MS of vis
                if outfile == '':
                    # These cmds came from the internal FLAG_CMD, only list on the screen
                    if inpmode == 'table' and inpfile == '':
                        pass
                    # Cmds came from external source, list on the screen
                    # and save them to MS and set APPLIED to False
                    else:
                        writeFlagCmd(vis, myflagcmd, vrows=keylist, applied=False)
                     
                # Append to a file   
                else:

                    ffout = open(outfile, 'a')

                    try:
                        casalog.post('Appending flag commands to '+outfile)
                        for cmd in mycmdlist:
                            print >> ffout, '%s' % cmd
                    except:
                        raise Exception, 'Error writing lines to file ' \
                            + outfile
                    ffout.close()
                

        # Apply/Unapply the flag commands to the data
        elif action == 'apply' or action == 'unapply':
            apply = True

            # Get the list of parameters
            for key in myflagcmd.keys():
                cmdline = myflagcmd[key]['command']
                cmdlist.append(cmdline)
                    
            # Get the union of all selection parameters
#            unionpars = getUnion(cmdlist)
            unionpars = fh.getUnion(cmdlist)
            casalog.post('The union of all parameters is %s' %(unionpars), 'DEBUG')

            # Select the data
            # Correlation will not go in here            
            tflocal.selectdata(unionpars)   

            # Parse the agents parameters
            if action == 'unapply':
                apply = False
                
            list2save = setupAgent(tflocal, myflagcmd, tablerows, apply)
            
            # If the display is requested, add it to list of agents
            if display != '':
                
                agent_pars = {}
                casalog.post('Parsing the display parameters')
                    
                agent_pars['mode'] = 'display'
                # need to create different parameters for both, data and report.
                if display == 'both':
                    agent_pars['datadisplay'] = True
                    agent_pars['reportdisplay'] = True
                
                elif display == 'data':
                    agent_pars['datadisplay'] = True
                
                elif display == 'report':
                    agent_pars['reportdisplay'] = True
                    
                tflocal.parseagentparameters(agent_pars)                

            # Initialize the Agents
            tflocal.init()
     
            # Backup the flags before running
            if flagbackup and writeflags:
                backupCmd(tflocal, list2save)
                
            # Run the tool
            stats = tflocal.run(writeflags, sequential)
                        
            tflocal.done()
            
            # Update the APPLIED column
            # These flags came from internal FLAG_CMD
            valid_rows = list2save.keys()  
            if inpmode == 'table' and inpfile == '':
                # do not need to save them as they are already in MS
                savepars = False
                myrowlist = myflagcmd.keys()
                if myrowlist.__len__() > 0:     
                    if not writeflags:
                        # APPLIED is set to False for any action
                        updateTable(vis, mycol='APPLIED', myval=False, myrowlist=valid_rows)
                    else:
                        # update APPLIED according to requested action
                        if apply:       
                            updateTable(vis, mycol='APPLIED', myval=True, myrowlist=valid_rows)
                        else:
                            updateTable(vis, mycol='APPLIED', myval=False, myrowlist=valid_rows)
                    
                                           
            # Save the valid command lines to the output file or FLAG_CMD
            if savepars:                           
                if valid_rows.__len__() > 0:                   
                    # save to MS
                    if outfile == '':
                        if not writeflags:
                            # APPLIED is set to False for any action
                            writeFlagCmd(vis, myflagcmd, valid_rows, writeflags)
                        else:
                            # Add the flag commands to the MS and update APPLIED
                            writeFlagCmd(vis, myflagcmd, valid_rows, applied=apply)                            
                           
                    # append to a file 
                    else:
                        ffout = open(outfile, 'a')
    
                        try:
                            casalog.post('Appending flag commands to '+outfile)
                            for key in list2save:
                                cmdline = list2save[key]['command']
                                print >> ffout, '%s' % cmdline
                        except:
                            raise Exception, 'Error writing lines to file ' \
                                + outfile
                        ffout.close()
                else:
                    casalog.post('There are no valid commands to save', 'WARN')
                 
                                            
        elif action == 'clear':
            # Clear flag commands from FLAG_CMD in vis
            msfile = vis

            if clearall:
                casalog.post('Deleting all rows from FLAG_CMD in MS '
                              + msfile)
                clearFlagCmd(msfile, myrowlist=rowlist)
            else:
                casalog.post('Safety Mode: you chose not to set clearall=True, no action'
                             )
                
        elif action == 'plot':

            keylist = myflagcmd.keys()
            if keylist.__len__() > 0:
                # Plot flag commands from FLAG_CMD or xml
                casalog.post('Warning: will only reliably plot individual per-antenna flags')
                plotflags(myflagcmd, plotfile, t1sdata, t2sdata)
            else:
                casalog.post('Warning: empty flag dictionary, nothing to plot')
        elif action == 'extract':
            # Output flag dictionary
            casalog.post('Returning extracted dictionary')
            return myflagcmd
        
    except Exception, instance:
        casalog.post('%s'%instance,'ERROR')
        raise


    # write history
    try:
        mslocal.open(vis, nomodify=False)
        mslocal.writehistory(message='taskname = tflagcmd',
                             origin='tflagcmd')
        mslocal.writehistory(message='vis      = "' + str(vis) + '"',
                             origin='testflagcmd')
        mslocal.writehistory(message='inpmode = "' + str(inpmode)
                             + '"', origin='tflagcmd')
        if inpmode == 'file':
            mslocal.writehistory(message='inpfile = "' + str(inpfile)
                                 + '"', origin='tflagcmd')
        elif inpmode == 'cmd':
            for cmd in command:
                mslocal.writehistory(message='command  = "' + str(cmd)
                        + '"', origin='tflagcmd')
        mslocal.close()
    except:
        casalog.post('Cannot open vis for history, ignoring', 'WARN')

    return

# ************************************************************************
#                    Helper Functions
# ************************************************************************

def getNumPar(cmdlist):
    '''Get the number of occurrences of all parameter keys
       -> cmdlist is a list of strings with parameters and values
    '''

    nrows = cmdlist.__len__()
            
    # Dictionary of number of occurrences to return
    npars = {
    'field':0,
    'scan':0,
    'antenna':0,
    'spw':0,
    'timerange':0,
    'correlation':0,
    'intent':0,
    'feed':0,
    'array':0,
    'uvrange':0,
    'comment':0
    }

    ci = 0  # count the number of lines with comments (starting with a #)
    si = 0  # count the number of lines with scan
    fi = 0  # count the number of lines with field
    ai = 0  # count the number of lines with antenna
    ti = 0  # count the number of lines with timerange
    coi = 0  # count the number of lines with correlation
    ii = 0  # count the number of lines with intent
    fei = 0  # count the number of lines with feed
    ari = 0  # count the number of lines with array
    ui = 0  # count the number of lines with yvrange
    pi = 0  # count the number of lines with spw
    
    for i in range(nrows):
        cmdline = cmdlist[i]

        if cmdline.startswith('#'):
            ci += 1
            npars['comment'] = ci
            continue

        # split by white space
        keyvlist = cmdline.split()
        if keyvlist.__len__() > 0:  

            # Split by '='
            for keyv in keyvlist:

                # Skip if it is a comment character #
                if keyv.count('#') > 0:
                    break

                (xkey,xval) = keyv.split('=')

                # Remove quotes
                if type(xval) == str:
                    if xval.count("'") > 0:
                        xval = xval.strip("'")
                    if xval.count('"') > 0:
                        xval = xval.strip('"')

                # Check which parameter
                if xkey == "scan":
                    si += 1
                    npars['scan'] = si

                elif xkey == "field":
                    fi += 1
                    npars['field'] = fi

                elif xkey == "antenna":
                    ai += 1
                    npars['antenna'] = ai

                elif xkey == "timerange":
                    ti += 1
                    npars['timerange'] = ti

                elif xkey == "correlation":
                    coi += 1
                    npars['correlation'] = coi

                elif xkey == "intent":
                    ii += 1
                    npars['intent'] = ii

                elif xkey == "feed":
                    fei += 1
                    npars['feed'] = fei

                elif xkey == "array":
                    ari += 1
                    npars['array'] = ari
                    arrays += xval + ','

                elif xkey == "uvrange":
                    ui += 1
                    npars['uvrange'] = ui

                elif xkey == "spw":
                    pi += 1
                    npars['spw'] = pi


    return npars


def getUnion(cmdlist):
    '''Get a dictionary of a union of all selection parameters from a list of lines:
       -> cmdlist is a list of strings with parameters and values
    '''
    
    # Dictionary of parameters to return
    dicpars = {
    'field':'',
    'scan':'',
    'antenna':'',
    'spw':'',
    'timerange':'',
    'correlation':'',
    'intent':'',
    'feed':'',
    'array':'',
    'uvrange':'',
    'observation':''
    }

    # Strings for each parameter
    scans = ''
    fields = ''
    ants = ''
    times = ''
    corrs = ''
    ints = ''
    feeds = ''
    arrays = ''
    uvs = ''
    spws = ''
    obs = ''
        
    nrows = cmdlist.__len__()

    for i in range(nrows):
        cmdline = cmdlist[i]
        
        # split by white space
        keyvlist = cmdline.split()
        if keyvlist.__len__() > 0:  

            # Split by '='
            for keyv in keyvlist:

                # Skip if it is a comment character #
#                if keyv.count('#') > 0:
                if keyv.startswith('#'):
                    break
                    
                (xkey,xval) = keyv.split('=')

                # Remove quotes
                if type(xval) == str:
                    if xval.count("'") > 0:
                        xval = xval.strip("'")
                    if xval.count('"') > 0:
                        xval = xval.strip('"')

                # Check which parameter
                if xkey == "scan":
                    scans += xval + ','

                elif xkey == "field":
                    fields += xval + ','

                elif xkey == "antenna":
                    ants += xval + ','

                elif xkey == "timerange":
                    times += xval + ','

                elif xkey == "correlation":
                    corrs += xval + ','

                elif xkey == "intent":
                    ints += xval + ','

                elif xkey == "feed":
                    feeds += xval + ','

                elif xkey == "array":
                    arrays += xval + ','

                elif xkey == "uvrange":
                    uvs += xval + ','

                elif xkey == "spw":
                    spws += xval + ','

                elif xkey == "observation":
                    obs += xval + ','

                        
    # Strip out the extra comma at the end
    scans = scans.rstrip(',')
    fields = fields.rstrip(',')
    ants = ants.rstrip(',')
    times = times.rstrip(',')
    corrs = corrs.rstrip(',')
    ints = ints.rstrip(',')
    feeds = feeds.rstrip(',')
    arrays = arrays.rstrip(',')
    uvs = uvs.rstrip(',')
    spws = spws.rstrip(',')
    obs = obs.rstrip(',')

    dicpars['scan'] = scans
    dicpars['field'] = fields
    dicpars['antenna'] = ants
    dicpars['timerange'] = times
    # Correlations should be handled only by the agents
    dicpars['correlation'] = ''
    dicpars['intent'] = ints
    dicpars['feed'] = feeds
    dicpars['array'] = arrays
    dicpars['uvrange'] = uvs
    dicpars['spw'] = spws
    dicpars['observation'] = obs

    # Real number of input lines
    # Get the number of occurrences of each parameter
    npars = getNumPar(cmdlist)
    nlines = nrows - npars['comment']
        
    # Make the union. 
    for k,v in npars.iteritems():
        if k != 'comment':
            if v < nlines:
                dicpars[k] = ''


    uniondic = dicpars.copy()
    # Remove empty parameters from the dictionary
    for k,v in dicpars.iteritems():
        if v == '':
            uniondic.pop(k)
    
    return uniondic


def delEmptyPars(cmdline):
    '''Remove empty parameters from a string:
       -> cmdline is a string with parameters
       returns a string containing only parameters with values
    '''
               
    newstr = ''
    
    # split by white space
    keyvlist = cmdline.split()
    if keyvlist.__len__() > 0:  
        
        # Split by '='
        for keyv in keyvlist:

            (xkey,xval) = keyv.split('=')

            # Remove quotes
            if type(xval) == str:
                if xval.count("'") > 0:
                    xval = xval.strip("'")
                if xval.count('"') > 0:
                    xval = xval.strip('"')
            
            # Write only parameters with values
            if xval == '':
                continue
            else:
                newstr = newstr+' '+xkey+'='+xval+' '
            
    else:
        casalog.post('String of parameters is empty','WARN')   
         
    return newstr


def setupAgent(tflocal, myflagcmd, myrows, apply):
    ''' Setup the parameters of each agent and call the tflagger tool
        myflagcmd --> it is a dictionary coming from readFromTable, readFromFile, etc.
        myrows --> selected rows to apply/unapply flags
        apply --> it's a boolean to control whether to apply or unapply the flags'''


    if not myflagcmd.__len__() >0:
        casalog.post('There are no flag cmds in list', 'SEVERE')
        return
    
    # Parameters for each mode
    manualpars = []
    clippars = ['clipminmax', 'expression', 'clipoutside','datacolumn', 'channelavg', 'clipzeros']
    quackpars = ['quackinterval','quackmode','quackincrement']
    shadowpars = ['diameter']
    elevationpars = ['lowerlimit','upperlimit'] 
    tfcroppars = ['ntime','combinescans','expression','datacolumn','timecutoff','freqcutoff',
                  'timefit','freqfit','maxnpieces','flagdimension','usewindowstats','halfwin']
    extendpars = ['ntime','combinescans','extendpols','growtime','growfreq','growaround',
                  'flagneartime','flagnearfreq']
    
        
    # dictionary of successful command lines to save to outfile
    savelist = {}

    # Setup the agent for each input line    
    for key in myflagcmd.keys():
        cmdline = myflagcmd[key]['command']
        applied = myflagcmd[key]['applied']
        casalog.post('cmdline for key%s'%key, 'DEBUG')
        casalog.post('%s'%cmdline, 'DEBUG')
        casalog.post('applied is %s'%applied, 'DEBUG')
        
        if cmdline.startswith('#'):
            continue
    
        modepars = {}
        parslist = {}
        mode = ''    
        valid = True
                
        # Get the specific parameters for the mode
        if cmdline.__contains__('mode'):                 
            if cmdline.__contains__('manual'): 
                mode = 'manual'
                modepars = getLinePars(cmdline,manualpars)   
            elif cmdline.__contains__('clip'):
                mode = 'clip'
                modepars = getLinePars(cmdline,clippars)
            elif cmdline.__contains__('quack'):
                mode = 'quack'
                modepars = getLinePars(cmdline,quackpars)
            elif cmdline.__contains__('shadow'):
                mode = 'shadow'
                modepars = getLinePars(cmdline,shadowpars)
            elif cmdline.__contains__('elevation'):
                mode = 'elevation'
                modepars = getLinePars(cmdline,elevationpars)
            elif cmdline.__contains__('tfcrop'):
                mode = 'tfcrop'
                modepars = getLinePars(cmdline,tfcroppars)
            elif cmdline.__contains__('extend'):
                mode = 'extend'
                modepars = getLinePars(cmdline,extendpars)
            elif cmdline.__contains__('unflag'):
                mode = 'unflag'
                modepars = getLinePars(cmdline,manualpars)
            elif cmdline.__contains__('rflag'):
                mode = 'rflag'
                modepars = getLinePars(cmdline,rflagpars)
            else:
                # Unknown mode, ignore it
                casalog.post('Ignoring unknown mode', 'WARN')
                valid = False

        else:
            # No mode means manual
            mode = 'manual'
            cmdline = cmdline+' mode=manual'
            modepars = getLinePars(cmdline,manualpars)   
                
                
        # Read ntime
        readNtime(modepars)
        
        # Cast the correct type to non-string parameters
        fixType(modepars)
        
        # Add the apply/unapply parameter to dictionary            
        modepars['apply'] = apply
        
        # Unapply selected rows only and re-apply the other rows with APPLIED=True
        if not apply and myrows.__len__() > 0:
            if key in myrows:
                modepars['apply'] = False
            elif not applied:
                casalog.post("Skipping this %s"%modepars,"DEBUG")
                continue
            elif applied:
                modepars['apply'] = True
                valid = False
        
        # Keep only cmds that overlap with the unapply cmds
        # TODO later
        
        # Hold the name of the agent and the cmd row number
        agent_name = mode.capitalize()+'_'+str(key)
        modepars['name'] = agent_name
        
        # Remove the data selection parameters if there is only one agent,
        # for performance reasons
        if myflagcmd.__len__() == 1:
            sellist=['scan','field','antenna','timerange','intent','feed','array','uvrange',
                     'spw','observation']
            for k in sellist:
                if modepars.has_key(k):
                    modepars.pop(k)

        casalog.post('Parsing parameters of mode %s in row %s'%(mode,key), 'DEBUG')
        casalog.post('%s'%modepars, 'DEBUG')

        # Parse the dictionary of parameters to the tool
        if (not tflocal.parseagentparameters(modepars)):
            casalog.post('Failed to parse parameters of mode %s in row %s' %(mode,key), 'WARN')
            continue
                            
        # Save the dictionary of valid agents
        if valid:               
            # add this command line to list to save in outfile
            parslist['row'] = key
            parslist['command'] = cmdline
            savelist[key] = parslist
        
        # FIXME: Backup the flags
#        if (flagbackup):
#            backup_cmdflags(tflocal, 'testflagcmd_' + mode)
    
    casalog.post('Dictionary of valid commands to save','DEBUG')
    casalog.post('%s'%savelist, 'DEBUG')
    
    return savelist


def getLinePars(cmdline, mlist=[]):
    '''Get a dictionary of all selection parameters from a line:
       -> cmdline is a string with parameters
       -> mlist is a list of the mode parameters to add to the
          returned dictionary.
    '''
            
    # Dictionary of parameters to return
    dicpars = {}
        
    # split by white space
    keyvlist = cmdline.split()
    if keyvlist.__len__() > 0:  
        
        # Split by '='
        for keyv in keyvlist:
            # Skip if it is a comment character #
            if keyv.count('#') > 0:
                break

            (xkey,xval) = keyv.split('=')

            # Remove quotes
            if type(xval) == str:
                if xval.count("'") > 0:
                    xval = xval.strip("'")
                if xval.count('"') > 0:
                    xval = xval.strip('"')

            # Check which parameter
            if xkey == "scan":
                dicpars['scan'] = xval

            elif xkey == "field":
                dicpars['field'] = xval

            elif xkey == "antenna":
                dicpars['antenna'] = xval

            elif xkey == "timerange":
                dicpars['timerange'] = xval

            elif xkey == "correlation":
                dicpars['correlation'] = xval

            elif xkey == "intent":
                dicpars['intent'] = xval

            elif xkey == "feed":
                dicpars['feed'] = xval

            elif xkey == "array":
                dicpars['array'] = xval

            elif xkey == "uvrange":
                dicpars['uvrange'] = xval

            elif xkey == "spw":
                dicpars['spw'] = xval
                
            elif xkey == "observation":
                dicpars['observation'] = xval

            elif xkey == "mode":
                dicpars['mode'] = xval

            elif mlist != []:
                # Any parameters requested for this mode?
                for m in mlist:
                    if xkey == m:
                        dicpars[m] = xval
                        
            
    return dicpars


def fixType(params):
    '''Give correct types to non-string parameters'''

    # quack parameters
    if params.has_key('quackmode') and not params['quackmode'] in ['beg'
            , 'endb', 'end', 'tail']:
        raise Exception, \
            "Illegal value '%s' of parameter quackmode, must be either 'beg', 'endb', 'end' or 'tail'" \
            % params['quackmode']
    if params.has_key('quackinterval'):
        params['quackinterval'] = float(params['quackinterval'])        
    if params.has_key('quackincrement'):
        if type(params['quackincrement']) == str:
            params['quackincrement'] = eval(params['quackincrement'].capitalize())

    # clip parameters
    if params.has_key('clipminmax'):
        value01 = params['clipminmax']
        # turn string into [min,max] range
        value0 = value01.lstrip('[')
        value = value0.rstrip(']')
        r = value.split(',')
        rmin = float(r[0])
        rmax = float(r[1])
        params['clipminmax'] = [rmin, rmax]        
    if params.has_key('clipoutside'):
        if type(params['clipoutside']) == str:
            params['clipoutside'] = eval(params['clipoutside'].capitalize())
        else:
            params['clipoutside'] = params['clipoutside']
    if params.has_key('channelavg'):
        params['channelavg'] = eval(params['channelavg'].capitalize())
    if params.has_key('clipzeros'):
        params['clipzeros'] = eval(params['clipzeros'].capitalize())
            
            
    # shadow parameter
    if params.has_key('diameter'):
        params['diameter'] = float(params['diameter'])
        
    # elevation parameters
    if params.has_key('lowerlimit'):
        params['lowerlimit'] = float(params['lowerlimit'])        
    if params.has_key('upperlimit'):
        params['upperlimit'] = float(params['upperlimit'])
        
    # extend parameters
    if params.has_key('extendpols'):        
        params['extendpols'] = eval(params['extendpols'].capitalize())
    if params.has_key('growtime'):
        params['growtime'] = float(params['growtime'])
    if params.has_key('growfreq'):
        params['growfreq'] = float(params['growfreq'])
    if params.has_key('growaround'):
        params['growaround'] = eval(params['growaround'].capitalize())
    if params.has_key('flagneartime'):
        params['flagneartime'] = eval(params['flagneartime'].capitalize())
    if params.has_key('flagnearfreq'):
        params['flagnearfreq'] = eval(params['flagnearfreq'].capitalize())

    # tfcrop parameters
    if params.has_key('combinescans'):
        params['combinescans'] = eval(params['combinescans'].capitalize())        
    if params.has_key('timecutoff'):
        params['timecutoff'] = float(params['timecutoff'])       
    if params.has_key('freqcutoff'):
        params['freqcutoff'] = float(params['freqcutoff'])        
    if params.has_key('maxnpieces'):
        params['maxnpieces'] = int(params['maxnpieces'])        
    if params.has_key('halfwin'):
        params['halfwin'] = int(params['halfwin'])
        

def readNtime(params):
    '''Check the value and units of ntime
       params --> dictionary of agent's parameters '''

    newtime = 0.0
    
    if params.has_key('ntime'):
        ntime = params['ntime']

        # Verify the ntime value
        if type(ntime) == float or type(ntime) == int:
            if ntime <= 0:
                raise Exception, 'Parameter ntime cannot be < = 0'
            else:
                # units are seconds
                newtime = float(ntime)
        
        elif type(ntime) == str:
            if ntime == 'scan':
                # iteration time step is a scan
                newtime = 0.0
            else:
                # read the units from the string
                qtime = qa.quantity(ntime)
                
                if qtime['unit'] == 'min':
                    # convert to seconds
                    qtime = qa.convert(qtime, 's')
                elif qtime['unit'] == '':
                    qtime['unit'] = 's'
                    
                # check units
                if qtime['unit'] == 's':
                    newtime = qtime['value']
                else:
                    casalog.post('Cannot convert units of ntime. Will use default 0.0s', 'WARN')
          
    params['ntime'] = float(newtime)
                  
            
def readFromTable(
    msfile,
    myflagrows=[],
    useapplied=True,
    myreason='',
    ):
    
    '''Read flag commands from rows of the FLAG_CMD table of msfile
    If useapplied=False then include only rows with APPLIED=False
    If myreason is anything other than '', then select on that'''
    
    #
    # Return flagcmd structure:
    #
    # The flagcmd key is the integer row number from FLAG_CMD
    #
    #   Dictionary structure:
    #   key : 'id' (string)
    #         'mode' (string)         flag mode '','clip','shadow','quack'
    #         'antenna' (string)
    #         'timerange' (string)
    #         'reason' (string)
    #         'time' (float)          in mjd seconds
    #         'interval' (float)      in mjd seconds
    #         'cmd' (string)          string (for COMMAND col in FLAG_CMD)
    #         'type' (string)         'FLAG' / 'UNFLAG'
    #         'applied' (bool)        set to True here on read-in
    #         'level' (int)           set to 0 here on read-in
    #         'severity' (int)        set to 0 here on read-in
    
    
    # Open and read columns from FLAG_CMD
    mstable = msfile + '/FLAG_CMD'

    # Note, tb.getcol doesn't allow random row access, read all
    
    try:
        tb.open(mstable)
        f_time = tb.getcol('TIME')
        f_interval = tb.getcol('INTERVAL')
        f_type = tb.getcol('TYPE')
        f_reas = tb.getcol('REASON')
        f_level = tb.getcol('LEVEL')
        f_severity = tb.getcol('SEVERITY')
        f_applied = tb.getcol('APPLIED')
        f_cmd = tb.getcol('COMMAND')
        tb.close()
    except:
        casalog.post("Error reading table "+ mstable, 'ERROR')
        raise Exception
    
    nrows = f_time.__len__()

    myreaslist = []
    
    # Parse myreason
    if type(myreason) == str:
        if myreason != '':
            myreaslist.append(myreason)
    elif type(myreason) == list:
        myreaslist = myreason
    else:
        casalog.post('Cannot read reason; it contains unknown variable types',
                     'ERROR')
        return

    myflagcmd = {}
    if nrows > 0:
        nflagd = 0
        if myflagrows.__len__() > 0:
            rowlist = myflagrows
        else:
            rowlist = range(nrows)
        # Prune rows if needed
        if not useapplied:
            rowl = []
            for i in rowlist:
                if not f_applied[i]:
                    rowl.append(i)
            rowlist = rowl
        if myreaslist.__len__() > 0:
            rowl = []
            for i in rowlist:
                if myreaslist.count(f_reas[i]) > 0:
                    rowl.append(i)
            rowlist = rowl

        for i in rowlist:
            flagd = {}
            flagd['id'] = str(i)
            flagd['antenna'] = ''
            flagd['mode'] = ''
            flagd['time'] = f_time[i]
            flagd['interval'] = f_interval[i]
            flagd['type'] = f_type[i]
            flagd['reason'] = f_reas[i]
            flagd['level'] = f_level[i]
            flagd['severity'] = f_severity[i]
            flagd['applied'] = f_applied[i]
            cmd = f_cmd[i]
            if cmd == '':
                casalog.post('Ignoring empty COMMAND string', 'WARN')
                continue
            flagd['command'] = cmd
            # Extract antenna and timerange strings from cmd
            antstr = ''
            timstr = ''
            keyvlist = cmd.split()
            if keyvlist.__len__() > 0:
                for keyv in keyvlist:
                    try:
                        (xkey, val) = keyv.split('=')
                    except:
                        print 'Error: not key=value pair for ' + keyv
                        break
                    xval = val
                # strip quotes from value
                    if xval.count("'") > 0:
                        xval = xval.strip("'")
                    if xval.count('"') > 0:
                        xval = xval.strip('"')
                    if xkey == 'timerange':
                        timstr = xval
                    elif xkey == 'antenna':
                        flagd['antenna'] = xval
                    elif xkey == 'id':
                        flagd['id'] = xval
                    elif xkey == 'mode':
                        flagd['mode'] = xval
            # STM 2010-12-08 Do not put timerange if not in command
            # if timstr=='':
            # ....    # Construct timerange from time,interval
            # ....    centertime = f_time[i]
            # ....    interval = f_interval[i]
            # ....    startmjds = centertime - 0.5*interval
            # ....    t = qa.quantity(startmjds,'s')
            # ....    starttime = qa.time(t,form="ymd",prec=9)
            # ....    endmjds = centertime + 0.5*interval
            # ....    t = qa.quantity(endmjds,'s')
            # ....    endtime = qa.time(t,form="ymd",prec=9)
            # ....    timstr = starttime+'~'+endtime
            flagd['timerange'] = timstr
            # Keep original key index, might need this later
            myflagcmd[i] = flagd
            nflagd += 1
        casalog.post('Read ' + str(nflagd)+ ' rows from FLAG_CMD table in '+msfile)
    else:
        casalog.post('FLAG_CMD table in %s is empty, no flags extracted'%msfile, 'WARN')

    return myflagcmd


def readFromCmd(cmdlist,ms_startmjds, ms_endmjds):
    '''Read the parameters from a list of commands'''

    # Read a list of strings and return a dictionary of parameters
    myflagd = {}
    nrows = cmdlist.__len__()
    if nrows == 0:
        casalog.post('WARNING: empty flag command list', 'WARN')
        return myflagd

    t = qa.quantity(ms_startmjds, 's')
    ms_startdate = qa.time(t, form=['ymd', 'no_time'])
    t0 = qa.totime(ms_startdate + '/00:00:00.0')
    # t0d = qa.convert(t0,'d')
    t0s = qa.convert(t0, 's')

    ncmds = 0
    for i in range(nrows):
        cmdstr = cmdlist[i]
        # break string into key=val sets
        keyvlist = cmdstr.split()
        if keyvlist.__len__() > 0:
            ant = ''
            timstr = ''
            tim = 0.5 * (ms_startmjds + ms_endmjds)
            intvl = ms_endmjds - ms_startmjds
            reas = ''
            cmd = ''
            fid = str(i)
            mode = ''
            typ = 'FLAG'
            appl = False
            levl = 0
            sevr = 0
            fmode = ''
            for keyv in keyvlist:
                # check for comment character #
                if keyv.count('#') > 0:
                    # break out of loop parsing keyvals
                    break
                try:
                    (xkey, val) = keyv.split('=')
                except:
                    print 'Not a key=val pair: ' + keyv
                    break
                xval = val
                # Use eval to deal with conversion from string
                # xval = eval(val)
                # strip quotes from value (if still a string)
                if type(xval) == str:
                    if xval.count("'") > 0:
                        xval = xval.strip("'")
                    if xval.count('"') > 0:
                        xval = xval.strip('"')

                # Strip these out of command string
                if xkey == 'reason':
                    reas = xval
                elif xkey == 'applied':
                    appl = False
                    if xval == 'True':
                        appl = True
                elif xkey == 'level':
                    levl = int(xval)
                elif xkey == 'severity':
                    sevr = int(xval)
                elif xkey == 'time':
                    tim = xval
                elif xkey == 'interval':
                    intvl = xval
                else:
                    # Extract (but keep in string)
                    if xkey == 'timerange':
                        timstr = xval
                        # Extract TIME,INTERVAL
                        try:
                            (startstr, endstr) = timstr.split('~')
                        except:
                            if timstr.count('~') == 0:
                            # print 'Assuming a single start time '
                                startstr = timstr
                                endstr = timstr
                            else:
                                print 'Not a start~end range: ' + timstr
                                print "ERROR: too may ~'s "
                                raise Exception, 'Error parsing ' \
                                    + timstr
                        t = qa.totime(startstr)
                        starts = qa.convert(t, 's')
                        if starts['value'] < 1.E6:
                            # assume a time offset from ref
                            starts = qa.add(t0s, starts)
                        startmjds = starts['value']
                        if endstr == '':
                            endstr = startstr
                        t = qa.totime(endstr)
                        ends = qa.convert(t, 's')
                        if ends['value'] < 1.E6:
                            # assume a time offset from ref
                            ends = qa.add(t0s, ends)
                        endmjds = ends['value']
                        tim = 0.5 * (startmjds + endmjds)
                        intvl = endmjds - startmjds
                    elif xkey == 'antenna':

                        ant = xval
                    elif xkey == 'id':
                        fid = xval
                    elif xkey == 'unflag':
                        if xval == 'True':
                            typ = 'UNFLAG'
                    elif xkey == 'mode':
                        fmode = xval
                    cmd = cmd + ' ' + keyv
            # Done parsing keyvals
            # Make sure there is a non-blank command string after reason/id extraction
            if cmd != '':
                flagd = {}
                flagd['id'] = fid
                flagd['mode'] = fmode
                flagd['antenna'] = ant
                flagd['timerange'] = timstr
                flagd['reason'] = reas
                flagd['command'] = cmd
                flagd['time'] = tim
                flagd['interval'] = intvl
                flagd['type'] = typ
                flagd['level'] = levl
                flagd['severity'] = sevr
                flagd['applied'] = appl
                # Insert into main dictionary
                myflagd[ncmds] = flagd
                ncmds += 1

    casalog.post('Parsed ' + str(ncmds) + ' flag command strings')

    return myflagd


def readFromFile(cmdlist, ms_startmjds, ms_endmjds, myreason=''):

    '''Parse list of flag command strings and return dictionary of flagcmds
    Inputs:
       cmdlist (list,string) list of command strings (default for TIME,INTERVAL)
       ms_startmjds (float)  starting mjd (sec) of MS (default for TIME,INTERVAL)
       ms_endmjds (float)    ending mjd (sec) of MS'''
#
#   Usage: myflagcmd = getflags(cmdlist)
#
#   Dictionary structure:
#   fid : 'id' (string)
#         'mode' (string)         flag mode '','clip','shadow','quack'
#         'antenna' (string)
#         'timerange' (string)
#         'reason' (string)
#         'time' (float)          in mjd seconds
#         'interval' (float)      in mjd seconds
#         'cmd' (string)          string (for COMMAND col in FLAG_CMD)
#         'type' (string)         'FLAG' / 'UNFLAG'
#         'applied' (bool)        set to True here on read-in
#         'level' (int)           set to 0 here on read-in
#         'severity' (int)        set to 0 here on read-in
#
# v3.2 Updated STM 2010-12-03 (3.2.0) handle comments # again
# v3.2 Updated STM 2010-12-08 (3.2.0) bug fixes in flagsort use, parsing
# v3.3 Updated STM 2010-12-20 (3.2.0) bug fixes parsing errors
#

    myflagd = {}
    nrows = cmdlist.__len__()
    if nrows == 0:
        casalog.post('WARNING: empty flag command list', 'WARN')
        return myflagd

    # Parse the reason
    reasonlist = []
    if type(myreason) == str:
        if myreason != '':
            reasonlist.append(myreason)
    elif type(myreason) == list:
        reasonlist = myreason
    else:
        casalog.post('Cannot read reason; it contains unknown variable types',
                     'ERROR')
        return

    t = qa.quantity(ms_startmjds, 's')
    ms_startdate = qa.time(t, form=['ymd', 'no_time'])
    t0 = qa.totime(ms_startdate + '/00:00:00.0')
    # t0d = qa.convert(t0,'d')
    t0s = qa.convert(t0, 's')

    rowlist = []
    
    # IMPLEMENT THIS LATER
    # First select by reason. Simple selection...
#    if reasonlist.__len__() > 0:
#        for i in range(nrows):
#            cmdstr = cmdlist[i]
#            keyvlist = cmdstr.split()
#            if keyvlist.__len__() > 0:
#                for keyv in keyvlist:
#                    (xkey, xval) = keyv.split('=')
#                    
#                    if type(xval) == str:
#                        if xval.count("'") > 0:
#                            xval = xval.strip("'")
#                        if xval.count('"') > 0:
#                            xval = xval.strip('"')
#    
#                    if xkey == 'reason':
#                        if reasonlist.count(xval) > 0
#    else:
    rowlist = range(nrows)
        
    
    # Now read the only the commands from the file that satisfies the reason selection

    ncmds = 0
#    for i in range(nrows):
    for i in rowlist:
        cmdstr = cmdlist[i]
                    
        # break string into key=val sets
        keyvlist = cmdstr.split()
        if keyvlist.__len__() > 0:
            ant = ''
            timstr = ''
            tim = 0.5 * (ms_startmjds + ms_endmjds)
            intvl = ms_endmjds - ms_startmjds
            reas = ''
            cmd = ''
            fid = str(i)
            mode = ''
            typ = 'FLAG'
            appl = False
            levl = 0
            sevr = 0
            fmode = ''
            for keyv in keyvlist:
                # check for comment character #
                if keyv.count('#') > 0:
                    # break out of loop parsing keyvals
                    break
                try:
                    (xkey, val) = keyv.split('=')
                except:
                    print 'Not a key=val pair: ' + keyv
                    break
                xval = val
                # Use eval to deal with conversion from string
                # xval = eval(val)
                # strip quotes from value (if still a string)
                if type(xval) == str:
                    if xval.count("'") > 0:
                        xval = xval.strip("'")
                    if xval.count('"') > 0:
                        xval = xval.strip('"')

                # Strip these out of command string
                if xkey == 'reason':
                    reas = xval
                                       
                elif xkey == 'applied':
                    appl = False
                    if xval == 'True':
                        appl = True
                elif xkey == 'level':
                    levl = int(xval)
                elif xkey == 'severity':
                    sevr = int(xval)
                elif xkey == 'time':
                    tim = xval
                elif xkey == 'interval':
                    intvl = xval
                else:
                    # Extract (but keep in string)
                    if xkey == 'timerange':
                        timstr = xval
                        # Extract TIME,INTERVAL
                        try:
                            (startstr, endstr) = timstr.split('~')
                        except:
                            if timstr.count('~') == 0:
                            # print 'Assuming a single start time '
                                startstr = timstr
                                endstr = timstr
                            else:
                                print 'Not a start~end range: ' + timstr
                                print "ERROR: too may ~'s "
                                raise Exception, 'Error parsing ' \
                                    + timstr
                        t = qa.totime(startstr)
                        starts = qa.convert(t, 's')
                        if starts['value'] < 1.E6:
                            # assume a time offset from ref
                            starts = qa.add(t0s, starts)
                        startmjds = starts['value']
                        if endstr == '':
                            endstr = startstr
                        t = qa.totime(endstr)
                        ends = qa.convert(t, 's')
                        if ends['value'] < 1.E6:
                            # assume a time offset from ref
                            ends = qa.add(t0s, ends)
                        endmjds = ends['value']
                        tim = 0.5 * (startmjds + endmjds)
                        intvl = endmjds - startmjds
                    elif xkey == 'antenna':

                        ant = xval
                    elif xkey == 'id':
                        fid = xval
                    elif xkey == 'unflag':
                        if xval == 'True':
                            typ = 'UNFLAG'
                    elif xkey == 'mode':
                        fmode = xval
                    cmd = cmd + ' ' + keyv
            # Done parsing keyvals
            # Make sure there is a non-blank command string after reason/id extraction
            if cmd != '':
                flagd = {}
                flagd['id'] = fid
                flagd['mode'] = fmode
                flagd['antenna'] = ant
                flagd['timerange'] = timstr
                flagd['reason'] = reas
                flagd['command'] = cmd
                flagd['time'] = tim
                flagd['interval'] = intvl
                flagd['type'] = typ
                flagd['level'] = levl
                flagd['severity'] = sevr
                flagd['applied'] = appl
                # Insert into main dictionary
                myflagd[ncmds] = flagd
                ncmds += 1

    casalog.post('Parsed ' + str(ncmds) + ' flag command strings')

    return myflagd


def readFromXML(sdmfile, mytbuff):

    '''readflagxml: reads Antenna.xml and Flag.xml SDM tables and parses
                 into returned dictionary as flag command strings
       sdmfile (string)  path to SDM containing Antenna.xml and Flag.xml
       mytbuff (float)   time interval (start and end) padding (seconds)'''
#
#   Usage: myflags = readflagxml(sdmfile,tbuff)
#
#   Dictionary structure:
#   fid : 'id' (string)
#         'mode' (string)         flag mode ('online')
#         'antenna' (string)
#         'timerange' (string)
#         'reason' (string)
#         'time' (float)          in mjd seconds
#         'interval' (float)      in mjd seconds
#         'cmd' (string)          string (for COMMAND col in FLAG_CMD)
#         'type' (string)         'FLAG' / 'UNFLAG'
#         'applied' (bool)        set to True here on read-in
#         'level' (int)           set to 0 here on read-in
#         'severity' (int)        set to 0 here on read-in
#

    try:
        from xml.dom import minidom
    except ImportError, e:
        print 'failed to load xml.dom.minidom:\n', e
        exit(1)

    if type(mytbuff) != float:
        casalog.post('Warning: incorrect type for tbuff, found "'
                     + str(mytbuff) + '", setting to 1.0')
        mytbuff = 1.0

    # construct look-up dictionary of name vs. id from Antenna.xml
    xmlants = minidom.parse(sdmfile + '/Antenna.xml')
    antdict = {}
    rowlist = xmlants.getElementsByTagName('row')
    for rownode in rowlist:
        rowname = rownode.getElementsByTagName('name')
        ant = str(rowname[0].childNodes[0].nodeValue)
        rowid = rownode.getElementsByTagName('antennaId')
        antid = str(rowid[0].childNodes[0].nodeValue)
        antdict[antid] = ant
    casalog.post('Found ' + str(rowlist.length)
                 + ' antennas in Antenna.xml')

    # now read Flag.xml into dictionary row by row
    xmlflags = minidom.parse(sdmfile + '/Flag.xml')
    flagdict = {}
    rowlist = xmlflags.getElementsByTagName('row')
    nrows = rowlist.length
    for fid in range(nrows):
        rownode = rowlist[fid]
        rowfid = rownode.getElementsByTagName('flagId')
        fidstr = str(rowfid[0].childNodes[0].nodeValue)
        flagdict[fid] = {}
        flagdict[fid]['id'] = fidstr
        rowid = rownode.getElementsByTagName('antennaId')
        antid = str(rowid[0].childNodes[0].nodeValue)
        antname = antdict[antid]
        # start and end times in mjd ns
        rowstart = rownode.getElementsByTagName('startTime')
        start = int(rowstart[0].childNodes[0].nodeValue)
        startmjds = float(start) * 1.0E-9 - mytbuff
        t = qa.quantity(startmjds, 's')
        starttime = qa.time(t, form='ymd', prec=9)
        rowend = rownode.getElementsByTagName('endTime')
        end = int(rowend[0].childNodes[0].nodeValue)
        endmjds = float(end) * 1.0E-9 + mytbuff
        t = qa.quantity(endmjds, 's')
        endtime = qa.time(t, form='ymd', prec=9)
    # time and interval for FLAG_CMD use
        times = 0.5 * (startmjds + endmjds)
        intervs = endmjds - startmjds
        flagdict[fid]['time'] = times
        flagdict[fid]['interval'] = intervs
        # reasons
        rowreason = rownode.getElementsByTagName('reason')
        reas = str(rowreason[0].childNodes[0].nodeValue)
        # Construct antenna name and timerange and reason strings
        flagdict[fid]['antenna'] = antname
        timestr = starttime + '~' + endtime
        flagdict[fid]['timerange'] = timestr
        flagdict[fid]['reason'] = reas
        # Construct command strings (per input flag)
        cmd = "antenna='" + antname + "' timerange='" + timestr + "'"
        flagdict[fid]['command'] = cmd
    #
        flagdict[fid]['type'] = 'FLAG'
        flagdict[fid]['applied'] = False
        flagdict[fid]['level'] = 0
        flagdict[fid]['severity'] = 0
        flagdict[fid]['mode'] = 'online'

    flags = {}
    if rowlist.length > 0:
        flags = flagdict
        casalog.post('Found ' + str(rowlist.length)
                     + ' flags in Flag.xml')
    else:
        casalog.post('No valid flags found in Flag.xml')

    # return the dictionary for later use
    return flags


def updateTable(
    msfile,
    mycol='',
    myval=None,
    myrowlist=[],
    ):
    
    '''Update commands in myrowlist of the FLAG_CMD table of msfile    
       Usage: updateflagcmd(msfile,myrow,mycol,myval)'''
    
    # Example:
    #
    #    updateflagcmd(msfile,mycol='APPLIED',myval=True)
    #       Mark all rows as APPLIED=True
    #
    #    updateflagcmd(msfile,mycol='APPLIED',myval=True,myrowlist=[0,1,2])
    #       Mark rows 0,1,2 as APPLIED=True
    #
    
    if mycol == '':
        casalog.post('WARNING: No column to was specified to update; doing nothing'
                     , 'WARN')
        return

    # Open and read columns from FLAG_CMD
    mstable = msfile + '/FLAG_CMD'
    try:
        tb.open(mstable, nomodify=False)
    except:
        raise Exception, 'Error opening table ' + mstable
    
    nrows = int(tb.nrows())

    
    # Check against allowed colnames
    colnames = tb.colnames()
    if colnames.count(mycol) < 1:
        casalog.post('Error: column mycol=' + mycol + ' not one of: '
                     + str(colnames))
        return

    nlist = myrowlist.__len__()
    
    if nlist > 0:
        rowlist = myrowlist

    else:
        rowlist = range(nrows)
        nlist = nrows
        
    if nlist > 0:
        try:
            tb.putcell(mycol, rowlist, myval)
        except:
            raise Exception, 'Error updating FLAG_CMD column ' + mycol \
                + ' to value ' + str(myval)

        casalog.post('Updated ' + str(nlist)
                     + ' rows of FLAG_CMD table in MS')
    tb.close()


def listFlagCmd(
    myflags=None,
    myantenna='',
    myreason='',
    myoutfile='',
    listmode='',
    ):
    
    '''List flags in myflags dictionary
    
    Format according to listmode:
        =''          do nothing
        ='file'      Format for flag command strings
        ='cmd'       Format for FLAG_CMD flags
        ='online'    Format for online flags'''
    
    #
    #   Dictionary structure:
    #   fid : 'id' (string)
    #         'mode' (string)         flag mode '','clip','shadow','quack'
    #         'antenna' (string)
    #         'timerange' (string)
    #         'reason' (string)
    #         'time' (float)          in mjd seconds
    #         'interval' (float)      in mjd seconds
    #         'cmd' (string)          string (for COMMAND col in FLAG_CMD)
    #         'type' (string)         'FLAG' / 'UNFLAG'
    #         'applied' (bool)
    #         'level' (int)
    #         'severity' (int)
    #
    
    useid = False
    doterm = False
    
    if myoutfile != '':
        try:
            lfout = open(myoutfile, 'w')
        except:
            raise Exception, 'Error opening list output file ' \
                + myoutfile

    keylist = myflags.keys()
    if keylist.__len__() == 0:
        casalog.post('There are no flags to list', 'WARN')
        return
    # Sort keys
#    keylist.sort
    
    # Set up any selection
    if myantenna != "":
        casalog.post('Selecting flags by antenna="' + str(myantenna) + '"')
    myantlist = myantenna.split(',')
    
    if myreason != '':
        casalog.post('Selecting flags by reason="' + str(myreason) + '"')
    myreaslist = myreason.split(',')

    if listmode == 'online':
        phdr = '%8s %12s %8s %32s %48s' % ('Key', 'FlagID', 'Antenna',
                'Reason', 'Timerange')
    elif listmode == 'cmd':
        phdr = '%8s %45s %32s %6s %7s %3s %3s %s' % (
            'Row',
            'Timerange',
            'Reason',
            'Type',
            'Applied',
            'Level',
            'Severity',
            'Command',
            )
    elif listmode == 'file':
        phdr = '%8s %s' % ('Key', 'Command')
    else:
        return
    
    if myoutfile != '':
        # list to output file
        print >> lfout, phdr
    else:
        # list to logger and screen
        if doterm:
            print phdr

        casalog.post(phdr)
        
    # Loop over flags
    for key in keylist:
        fld = myflags[key]
        # Get fields
        skey = str(key)
        if fld.has_key('id') and useid:
            fid = fld['id']
        else:
            fid = str(key)
        if fld.has_key('antenna'):
            ant = fld['antenna']
        else:
            ant = 'Unset'
        if fld.has_key('timerange'):
            timr = fld['timerange']
        else:
            timr = 'Unset'
        if fld.has_key('reason'):
            reas = fld['reason']
        else:
            reas = 'Unset'
        if fld.has_key('command'):
            cmd = fld['command']
        else:
            cmd = 'Unset'
        if fld.has_key('type'):
            typ = fld['type']
        else:
            typ = 'FLAG'
        if fld.has_key('level'):
            levl = str(fld['level'])
        else:
            levl = '0'
        if fld.has_key('severity'):
            sevr = str(fld['severity'])
        else:
            sevr = '0'
        if fld.has_key('applied'):
            appl = str(fld['applied'])
        else:
            appl = 'Unset'
            
        # Print out listing
        if myantenna == '' or myantlist.count(ant) > 0:
            if myreason == '' or myreaslist.count(reas) > 0:
                if listmode == 'online':
                    pstr = '%8s %12s %8s %32s %48s' % (skey, fid, ant,
                            reas, timr)
                elif listmode == 'cmd':
                    pstr = '%8s %45s %32s %6s %7s %3s %3s %s' % (
                        skey,
                        timr,
                        reas,
                        typ,
                        appl,
                        levl,
                        sevr,
                        cmd,
                        )
                else:
                    pstr = '%8s %s' % (skey, cmd)
                if myoutfile != '':
                    # list to output file
                    print >> lfout, pstr
                else:
                    # list to logger and screen
                    if doterm:
                        print pstr
                    casalog.post(pstr)
    if myoutfile != '':
        lfout.close()
    

def backupCmd(tflocal, cmdlist):

        # Create names like this:
        # before_tflagcmd_1,
        # before_tflagcmd_2,
        #
        # Generally  before_<mode>_<i>, where i is the smallest
        # integer giving a name, which does not already exist

    prefix = 'tflagcmd'
    existing = tflocal.getflagversionlist(printflags=True)

    # remove comments from strings
    existing = [x[0:x.find(' : ')] for x in existing]
    i = 1
    while True:
        versionname = prefix + '_' + str(i)

        if not versionname in existing:
            break
        else:
            i = i + 1

    time_string = str(time.strftime('%Y-%m-%d %H:%M:%S'))

    casalog.post('Saving current flags to ' + versionname
                 + ' before applying new flags')

    tflocal.saveflagversion(versionname=versionname,
                            comment='tflagcmd autosave on ' + time_string, merge='replace')




def writeFlagCmd(msfile, myflags, vrows, applied):
    '''
    Writes the flag commands to FLAG_CMD or to an ASCII file
    msfile    MS
    myflags   dictionary of commands read from inpfile (from readFromTable, etc.)
    vrows     list of valid rows from myflags dictionary to save
    tag       tag to update APPLIED column of FLAG_CMD
        Returns the number of commands written to output
    '''
    
    nadd = 0
    try:
        import pylab as pl
    except ImportError, e:
        print 'failed to load pylab:\n', e
        exit(1)
    #
    # Append new commands to existing table
    
    if vrows.__len__() > 0:
        # Extract flags from dictionary into list
        tim_list = []
        intv_list = []
        cmd_list = []
        reas_list = []
        typ_list = []
        sev_list = []
        lev_list = []
        app_list = []
        
        # Only write valid rows that have been applied to MS
        for key in vrows:
            tim_list.append(myflags[key]['time'])
            intv_list.append(myflags[key]['interval'])
            reas_list.append(myflags[key]['reason'])
            
            # if no mode, add the default
            command = myflags[key]['command']
            if not command.__contains__('mode'):
                myflags[key]['command'] = command+' mode=manual'
            cmd_list.append(myflags[key]['command'])
            typ_list.append(myflags[key]['type'])
            sev_list.append(myflags[key]['severity'])
            lev_list.append(myflags[key]['level'])
            app_list.append(applied)
    
        # Save to FLAG_CMD table
        nadd = cmd_list.__len__()

        mstable = msfile + '/FLAG_CMD'
        try:
            tb.open(mstable, nomodify=False)
        except:
            raise Exception, 'Error opening FLAG_CMD table ' + mstable
        nrows = int(tb.nrows())
        casalog.post('There are ' + str(nrows)
                     + ' rows already in FLAG_CMD')
        # add blank rows
        tb.addrows(nadd)
        # now fill them in
        tb.putcol('TIME', pl.array(tim_list), startrow=nrows, nrow=nadd)
        tb.putcol('INTERVAL', pl.array(intv_list), startrow=nrows,
                  nrow=nadd)
        tb.putcol('REASON', pl.array(reas_list), startrow=nrows,
                  nrow=nadd)
        tb.putcol('COMMAND', pl.array(cmd_list), startrow=nrows,
                  nrow=nadd)
        # Other columns
        tb.putcol('TYPE', pl.array(typ_list), startrow=nrows, nrow=nadd)
        tb.putcol('SEVERITY', pl.array(sev_list), startrow=nrows,
                  nrow=nadd)
        tb.putcol('LEVEL', pl.array(lev_list), startrow=nrows,
                  nrow=nadd)
        tb.putcol('APPLIED', pl.array(app_list), startrow=nrows,
                  nrow=nadd)
        tb.close()
    
        casalog.post('Saved ' + str(nadd) + ' rows to FLAG_CMD')
        
    else:
        casalog.post('Saved zero rows to FLAG_CMD; no flags found')

    return nadd
            

def clearFlagCmd(msfile, myrowlist=[]):
    #
    # Delete flag commands (rows) from the FLAG_CMD table of msfile
    #
    # Open and read columns from FLAG_CMD
    
    mstable = msfile + '/FLAG_CMD'
    try:
        tb.open(mstable, nomodify=False)
    except:
        raise Exception, 'Error opening table ' + mstable
    
    nrows = int(tb.nrows())
    casalog.post('There were ' + str(nrows) + ' rows in FLAG_CMD')
    if nrows > 0:
        if myrowlist.__len__() > 0:
            rowlist = myrowlist
        else:
            rowlist = range(nrows)
        try:
            tb.removerows(rowlist)
            casalog.post('Deleted ' + str(rowlist.__len__())
                         + ' from FLAG_CMD table in MS')
        except:
            tb.close()
            raise Exception, 'Error removing rows ' + str(rowlist) \
                + ' from table ' + mstable

        nnew = int(tb.nrows())
    else:
        casalog.post('No rows to clear')
        
    tb.close()


def plotflags(
    myflags,
    plotname,
    t1sdata,
    t2sdata,
    ):
    
    try:
        import casac
    except ImportError, e:
        print 'failed to load casa:\n', e
        exit(1)
    qatool = casac.homefinder.find_home_by_name('quantaHome')
    qa = casac.qa = qatool.create()

    try:
        import pylab as pl
    except ImportError, e:
        print 'failed to load pylab:\n', e
        exit(1)

    # get list of flag keys
    keylist = myflags.keys()

    # get list of antennas
    myants = []
    for key in keylist:
        ant = myflags[key]['antenna']
        if myants.count(ant) == 0:
            myants.append(ant)
    myants.sort()

    antind = 0
    if plotname == '':
        pl.ion()
    else:
        pl.ioff()

    f1 = pl.figure()
    ax1 = f1.add_axes([.15, .1, .75, .85])
#    ax1.set_ylabel('antenna')
#    ax1.set_xlabel('time')
    badflags = []
    for thisant in myants:
        antind += 1
        for thisflag in myflags:
            if myflags[thisflag]['antenna'] == thisant:
            # print thisant, myflags[thisflag]['reason'], myflags[thisflag]['timerange']
                thisReason = myflags[thisflag]['reason']
                if thisReason == 'FOCUS_ERROR':
                    thisColor = 'red'
                    thisOffset = 0.29999999999999999
                elif thisReason == 'SUBREFLECTOR_ERROR':
                    thisColor = 'blue'
                    thisOffset = .15
                elif thisReason == 'ANTENNA_NOT_ON_SOURCE':
                    thisColor = 'green'
                    thisOffset = 0
                elif thisReason == 'ANTENNA_NOT_IN_SUBARRAY':
                    thisColor = 'black'
                    thisOffset = -.15
                else:
                    thisColor = 'orange'
                    thisOffset = 0.29999999999999999
                mytimerange = myflags[thisflag]['timerange']
                if mytimerange != '':
                    t1 = mytimerange[:mytimerange.find('~')]
                    t2 = mytimerange[mytimerange.find('~') + 1:]
                    (t1s, t2s) = (qa.convert(t1, 's')['value'],
                                  qa.convert(t2, 's')['value'])
                else:
                    t1s = t1sdata
                    t2s = t2sdata
                myTimeSpan = t2s - t1s
                if myTimeSpan < 10000:
                    ax1.plot([t1s, t2s], [antind + thisOffset, antind
                             + thisOffset], color=thisColor, lw=2,
                             alpha=.7)
                else:
                    badflags.append((thisant, myTimeSpan, thisReason))

    # #badflags are ones which span a time longer than that used above
    # #they can be so long that including them compresses the time axis so that none of the other flags are visible
#    print 'badflags', badflags
    myXlim = ax1.get_xlim()
    myXrange = myXlim[1] - myXlim[0]
    legendFontSize = 12
    ax1.text(myXlim[0] + .05 * myXrange, 29, 'FOCUS', color='red',
             size=legendFontSize)
    ax1.text(myXlim[0] + .17 * myXrange, 29, 'SUBREFLECTOR',
             color='blue', size=legendFontSize)
    ax1.text(myXlim[0] + .42 * myXrange, 29, 'OFF SOURCE', color='green'
             , size=legendFontSize)
    ax1.text(myXlim[0] + .62 * myXrange, 29, 'NOT IN SUBARRAY',
             color='black', size=legendFontSize)
    ax1.text(myXlim[0] + .90 * myXrange, 29, 'Other', color='orange',
             size=legendFontSize)
    ax1.set_ylim([0, 30])

    ax1.set_yticks(range(1, len(myants) + 1))
    ax1.set_yticklabels(myants)
    ax1.set_xticks(pl.linspace(myXlim[0], myXlim[1], 3))

    mytime = [myXlim[0], (myXlim[1] + myXlim[0]) / 2.0, myXlim[1]]
    myTimestr = []
    for time in mytime:
        q1 = qa.quantity(time, 's')
        time1 = qa.time(q1, form='ymd', prec=9)
        myTimestr.append(time1)

    ax1.set_xticklabels([myTimestr[0], (myTimestr[1])[11:],
                        (myTimestr[2])[11:]])
    # print myTimestr
    if plotname == '':
        pl.draw()
    else:
        pl.savefig(plotname, dpi=150)
    return

        
    
    
    
        
    