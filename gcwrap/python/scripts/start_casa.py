import os
import sys
import traceback
if os.environ.has_key('LD_PRELOAD'):
    del os.environ['LD_PRELOAD']

from IPython import start_ipython

__pylib = os.path.dirname(os.path.realpath(__file__))
__init_scripts = [
    "init_system.py",
    "init_logger.py",
    "init_user_pre.py",
    "init_dbus.py",
    "init_tools.py",
    "init_tasks.py",
    "init_funcs.py",
    "init_asap.py",
    "init_pipeline.py",
    "init_mpi.py",
    "init_testing.py",
    "init_docs.py",
    "init_user_post.py",
    "init_crashrpt.py",
    "init_welcome.py",
]

##
## this is filled via register_builtin (from casa_builtin.py)
##
casa_builtins = { }

##
## this is filled via add_shutdown_hook (from casa_shutdown.py)
##
casa_shutdown_handlers = [ ]

try:
    __startup_scripts = filter( os.path.isfile, map(lambda f: __pylib + '/' + f, __init_scripts ) )
    # '-nomessages', '-nobanner','-logfile',ipythonlog,'-ipythondir',casa['dirs']['rc']+'/ipython'
    start_ipython( argv=['--autocall=2', "-c", "for i in " + str(__startup_scripts) + ": execfile( i )\n%matplotlib", "-i" ] )
except:
    print "Unexpected error:", sys.exc_info()[0]
    traceback.print_exc(file=sys.stdout)
    pass


### this should (perhaps) be placed in an 'atexit' hook...
for handler in casa_shutdown_handlers:
    handler( )
