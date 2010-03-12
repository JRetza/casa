#
# CASA - Common Astronomy Software Applications
# Copyright (C) 2010 by ESO (in the framework of the ALMA collaboration)
#
# This file is part of CASA.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

#
# Macros related to CASA's user interface
#


#
#  Rule for handling task XML
#
#     casa_add_tasks( target
#                     source1.xml [source2.xml ...] )
#
#  - Generates python bindings
#  - Generates tasks.py taskinfo.py
#  - Defines install rules
#

macro( casa_add_tasks _target )
  set( _xmls ${ARGN} )

  set( _out_all "" )

  foreach( _x ${_xmls} )
    
    get_filename_component( _base ${_x} NAME_WE )
    get_filename_component( _xml ${_x} ABSOLUTE )

    set( _cli ${CMAKE_CURRENT_BINARY_DIR}/${_base}_cli.py )
    set( _pg  ${CMAKE_CURRENT_BINARY_DIR}/${_base}_pg.py )
    set( _py  ${CMAKE_CURRENT_BINARY_DIR}/${_base}.py )

    # Create _cli.py
    set( _xsl ${CMAKE_SOURCE_DIR}/xmlcasa/install/casa2pycli.xsl )
    add_custom_command(
      OUTPUT ${_cli}
      COMMAND ${SAXON} -o ${_cli} ${_xml} ${_xsl} 
      DEPENDS ${_xml} ${_xsl} )

    # Create _pg.py
    set( _xsl ${CMAKE_SOURCE_DIR}/xmlcasa/install/casa2pypg.xsl )
    add_custom_command(
      OUTPUT ${_pg}
      COMMAND ${SAXON} -o ${_pg} ${_xml} ${_xsl} 
      DEPENDS ${_xml} ${_xsl} )

    # Create .py
    set( _xsl ${CMAKE_SOURCE_DIR}/xmlcasa/install/casa2pyimp.xsl )
    add_custom_command(
      OUTPUT ${_py}
      COMMAND ${SAXON} -o ${_py} ${_xml} ${_xsl}
      DEPENDS ${_xml} ${_xsl} )

    # Keep track of generated files
    set( _out_all ${_out_all} ${_py} ${_cli} ${_pg} )

  endforeach()

  add_custom_target( 
    ${_target}
    ALL 
    DEPENDS ${_out_all} )

  install( 
    FILES ${_xmls}
    DESTINATION ../share/xml
    )

  install( 
    PROGRAMS ${_out_all}
    DESTINATION python/${PYTHONV} 
    )


  # Create tasks.py, tasksinfo.py
  # This is a lousy implementation, which
  # runs saxon on every .xml file, if any of them changed.
  set( _tasks     ${CMAKE_CURRENT_BINARY_DIR}/tasks.py )
  set( _tasksinfo ${CMAKE_CURRENT_BINARY_DIR}/tasksinfo.py )
  set( _xsl ${CMAKE_SOURCE_DIR}/xmlcasa/install/casa2tsum2.xsl )
  add_custom_command( 
    OUTPUT ${_tasks} ${_tasksinfo}
    COMMAND echo > taskinfo
    COMMAND for task in ${CMAKE_CURRENT_SOURCE_DIR}/tasks/*.xml \; do echo \$\$task \;  ${SAXON} \$\$task ${_xsl} >> taskinfo \;  done
    COMMAND echo >> taskinfo
    COMMAND echo "from tget import *" >> taskinfo
    COMMAND echo "from taskmanager import tm" >> taskinfo
    COMMAND grep "^from" taskinfo > ${_tasks}
    COMMAND echo "from odict import odict" > ${_tasksinfo}
    COMMAND echo "mytasks = odict\\(\\)" >> ${_tasksinfo}
    COMMAND echo "tasksum = odict\\(\\)" >> ${_tasksinfo}
    COMMAND grep -Ev "^#?from" taskinfo >> ${_tasksinfo}
    DEPENDS ${_xmls}
    )

  add_custom_target( tasks ALL DEPENDS ${_tasks} ${_tasksinfo} )

  install(
    PROGRAMS ${_tasks} ${_tasksinfo} DESTINATION python/${PYTHONV}
    )
endmacro()


#
#   casa_ccmtools( INPUT input1 [input2 ...]
#                  OUTPUT output1 [output2 ...]
#                  OPTIONS option1 [option2 ...] 
#                  DEPENDS depend1 [depend2 ...] )
#
#  There are a few issues involved with invoking ccmtools.
#  The solution to these are factored out in this macro.
#
#  - ccmtools sometimes skips writing an output file. 
#    This has the consequence that if the file was considered
#    out of date, it will still be considered out of date.
#    This is resolved by removing all expected output files
#    before invoking ccmtools
#  - ccmtools always returns zero, even in case of error. In
#    order for the build system to check if the invocation was
#    successful, it is checked that all output files exist after
#    running ccmtools
#  - implicit dependencies from the generated C++ to IDL files
#    (which include each other).
#    The option IMPLICIT_DEPENDS traces the dependencies
#    from the output C++ to indirect IDL files included by
#    the direct dependency. Note, in CMake 2.8 this works only
#    for the Makefile backend.

macro( casa_ccmtools )
  # Careful!: variable names have global scope.
  # The names must not conflict with 'temporary' 
  # variables used in the caller macro. This is
  # the responsibility of the calling code.

  # Parse options
  set( _opt false )
  set( _in false )
  set( _out false )
  set( _dep false )
  set( _input "" )
  set( _output "" )
  set( _options "" )
  set( _depends "" )
  foreach( _arg ${ARGN} )
    if( _arg STREQUAL OPTIONS)
      set( _opt true )
      set( _in false )
      set( _out false )
      set( _dep false )
    elseif( _arg STREQUAL INPUT)
      set( _opt false )
      set( _in true )
      set( _out false )
      set( _dep false )
    elseif( _arg STREQUAL OUTPUT)
      set( _opt false )
      set( _in false )
      set( _out true )
      set( _dep false )
    elseif( _arg STREQUAL DEPENDS)
      set( _opt false )
      set( _in false )
      set( _out false )
      set( _dep true )
    elseif( _opt )
      set( _options ${_options} ${_arg} )
    elseif( _in )
      set( _input ${_input} ${_arg} )
    elseif( _out )
      set( _output ${_output} ${_arg} )
    elseif( _dep )
      set( _depends ${_depends} ${_arg} )
    else(
        message( FATAL_ERROR "Illegal options: ${ARGN}" )
        )
    endif()
  endforeach()
  # Done parsing

  #message(" OPTIONS = ${_options} ")
  #message(" INPUT = ${_input} ")
  #message(" OUTPUT = ${_output} ")

  set( _conversions ${CMAKE_SOURCE_DIR}/xmlcasa/xml/conversions.xml )

  add_custom_command(
    OUTPUT ${_output}

    COMMAND rm -f ${_output}  # otherwise CCMTOOLS sometimes skips writing

    COMMAND ${CCMTOOLS_ccmtools_EXECUTABLE} ${CCMTOOLS_ccmtools_FLAGS} ${_options}
    --overwrite 
    --coda=${_conversions}
    -I${CMAKE_SOURCE_DIR}/xmlcasa/idl
    -I${CMAKE_CURRENT_BINARY_DIR}
    ${_input}

    # Now check if the outputs were created and return an error if not
    COMMAND ${PERL_EXECUTABLE} -le 'for (@ARGV) { ( -e ) or die \"$$_ missing!\"\; }' --
    ARGS ${_output}

    DEPENDS ${_conversions} ${_depends}

    # Indirect dependencies to included IDL files
    IMPLICIT_DEPENDS C ${_depends}  # Use the C preprocessor because
                                    # that is what ccmtools uses
    )
  
  if( NOT CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    message( WARNING "Dependency tracking of generated IDL files may not work when CMAKE_GENERATOR is ${CMAKE_GENERATOR}" )
  endif()

endmacro()


#
#  Rules for handling tool XML
#
#        casa_add_tools( out_idl out_sources
#                        tool1.xml [tool2.xml ...]
#                      )
#
#  out_idl: generated IDL
#  out_sources: generated .cc and .h files
#

macro( casa_add_tools out_idl out_sources )

  set( _xmls ${ARGN} )

  foreach( _x ${_xmls} )
    
    get_filename_component( _base ${_x} NAME_WE )
    get_filename_component( _xml ${_x} ABSOLUTE )

    # Generate .xml from .xml
    set( _out_xml casa${_base}.xml )
    set( _xsl ${CMAKE_SOURCE_DIR}/xmlcasa/install/casa2toolxml.xsl )
    add_custom_command(
      OUTPUT ${_out_xml}
      COMMAND ${SAXON} ${_xml} ${_xsl} | sed -e \"s/exmlns/xmlns/\" > ${_out_xml}
      DEPENDS ${_xml} ${_xsl} 
      )

    # Then xml -> idl
    set( _idl ${CMAKE_CURRENT_BINARY_DIR}/casa${_base}.idl )
    set( _xsl ${CMAKE_SOURCE_DIR}/xmlcasa/install/casa2idl3.xsl )
    add_custom_command(
      OUTPUT ${_idl}
      COMMAND ${SAXON} ${_out_xml} ${_xsl} | sed -e \"s/<?xml version=.*//\" > ${_idl}
      DEPENDS ${_out_xml} ${_xsl} 
      )

    # CCMTools create C++ bindings from IDL
    set( _outputs 
      Python_Converter/casa${_base}_python.cc
      Python_Converter/CCM_Local/casac/CCM_Session_${_base}/${_base}Home_python.cc
      Python_Converter/CCM_Local/casac/CCM_Session_${_base}/${_base}_python.cc
      Python_Converter/CCM_Local/casac/CCM_Session_${_base}/${_base}_python.h
      Python_Converter/CCM_Local/casac/CCM_Session_${_base}/${_base}Home_python.h
      CCM_Local/casac/CCM_Session_${_base}/${_base}_gen.cc
      CCM_Local/casac/CCM_Session_${_base}/${_base}Home_gen.cc
      CCM_Local/casac/CCM_Session_${_base}/${_base}Home_share.h
      CCM_Local/casac/CCM_Session_${_base}/${_base}_gen.h
      CCM_Local/casac/CCM_Session_${_base}/${_base}Home_gen.h
      CCM_Local/casac/CCM_Session_${_base}/${_base}_share.h
      impl/${_base}_impl.cc
      impl/${_base}Home_impl.cc
      impl/${_base}_impl.h
      impl/${_base}Home_impl.h
      impl/${_base}_cmpt.h
      impl/${_base}Home_entry.h
      )

    casa_ccmtools(
      INPUT ${_idl}
      OUTPUT ${_outputs}
      OPTIONS c++local c++python
      DEPENDS ${_idl}
      )

    set( ${out_idl} ${${out_idl}} ${_idl} )

    set( ${out_sources} ${${out_sources}} ${_outputs} )

  endforeach()
  
endmacro()


#
#  Rules for handling source IDL
#
#       casa_idl( outfiles input
#                 prefix1 [prefix2 ...] )
#
#  outfiles: list of generated C++ sources
#  input   : source IDL
#  prefix* : basename of output C++ sources
#

macro( casa_idl outfiles input )

    set( _types ${ARGN} )  # Created output files

    get_filename_component( _idl ${input} ABSOLUTE )

    set( _outputs "" )

    foreach( _t ${_types} )     
      set( _outputs ${_outputs}
        Python_Converter/${_t}_python.h
        Python_Converter/${_t}_python.cc
        )

    endforeach()
     
    casa_ccmtools(
      INPUT ${_idl}
      OUTPUT ${_outputs}
      OPTIONS c++local c++python
      DEPENDS ${_idl}
      )
    
    set( ${outfiles} ${${outfiles}} ${_outputs} )
    
endmacro()


#
#  casa_pybinding( outfiles
#                  source1.idl [source2.idl] )
#
#  outfiles: generated casac_python.* C++ files
#  source* : input IDLs
#

macro( casa_pybinding outfiles )

  set( _idls ${ARGN} )

  # Avoid name clashing between the casac_python.*
  # created here and elsewhere (by other ccmtools invocations) by 
  # using a different output directory
  set( ${outfiles} 
    pybinding/Python_Converter/casac_python.cc
    pybinding/Python_Converter/casac_python.h
    )

  casa_ccmtools(
    INPUT ${_idls}
    OUTPUT ${${outfiles}}
    OPTIONS c++local c++python
    --output=${CMAKE_CURRENT_BINARY_DIR}/pybinding
    )
    # You would think that here is missing a DEPENDS ${_idls},
    # but in fact the output does not depend on the content of the
    # input files, and therefore does *not* need to be regenerated 
    # whenever an input file changes (which takes much time). The
    # output only depends on the input filenames.

endmacro()
