# - Find log4cxx
#
# This script sets the following variables:
# LOG4CXX_FOUND - true if log4cxx is found
# LOG4CXX_LIBRARIES - a list of libraries to use log4cxx package
# LOG4CXX_INCLUDE_DIRS - a list of include directories for header files
#
FIND_PATH(LOG4CXX_INCLUDE_DIR log4cxx/logger.h PATHS /usr/include /usr/local/include /opt/log4cxx/include)
FIND_LIBRARY(LOG4CXX_LIBRARY NAMES log4cxx log4cxxd PATHS /lib64 /lib /usr/lib64 /usr/lib /usr/local/lib64 /usr/local/lib /opt/log4cxx/lib64 /opt/log4cxx/lib)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LOG4CXX DEFAULT_MSG LOG4CXX_LIBRARY LOG4CXX_INCLUDE_DIR)

IF(LOG4CXX_FOUND)
  SET( LOG4CXX_LIBRARIES ${LOG4CXX_LIBRARY} )
  SET( LOG4CXX_INCLUDE_DIRS ${LOG4CXX_INCLUDE_DIR} )
ELSE(LOG4CXX_FOUND)
  SET( LOG4CXX_LIBRARIES )
  SET( LOG4CXX_INCLUDE_DIRS )
ENDIF(LOG4CXX_FOUND)

MARK_AS_ADVANCED(LOG4CXX_INCLUDE_DIRS LOG4CXX_LIBRARIES)
