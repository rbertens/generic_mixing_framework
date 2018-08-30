# **************************************************************************
# * Copyright(c) 1998-2014, ALICE Experiment at CERN, All rights reserved. *
# *                                                                        *
# * Author: The ALICE Off-line Project.                                    *
# * Contributors are mentioned in the code where appropriate.              *
# *                                                                        *
# * Permission to use, copy, modify and distribute this software and its   *
# * documentation strictly for non-commercial purposes is hereby granted   *
# * without fee, provided that the above copyright notice appears in all   *
# * copies and that both the copyright notice and this permission notice   *
# * appear in the supporting documentation. The authors make no claims     *
# * about the suitability of this software for any purpose. It is          *
# * provided "as is" without express or implied warranty.                  *
# **************************************************************************

# General purpose functions

#########################
# ROOT utilities
#########################

# Generation of the dictionaries
# @DNAME  Dictionary name
# @LDNAME LinkDef file name, ex: LinkDef.h
# @DHDRS  Dictionary headers
# @DINCDIR Include folders that need to be passed to cint/cling
# @EXTRADEFINITIONS - optional, extra compile flags specific to library
#       - used as ${ARGV4}
macro(generate_dictionary DNAME LDNAME DHDRS DINCDIRS)

    # Creating the INCLUDE path for cint/cling
    foreach( dir ${DINCDIRS})
        set(INCLUDE_PATH -I${dir} ${INCLUDE_PATH})
    endforeach()
    
    # Get the list of definitions from the directory to be sent to CINT
    get_directory_property(tmpdirdefs COMPILE_DEFINITIONS)
    foreach(dirdef ${tmpdirdefs})
        set(GLOBALDEFINITIONS -D${dirdef} ${GLOBALDEFINITIONS})
    endforeach()
    
    # Custom definitions specific to library
    # Received as the forth optional argument
    separate_arguments(EXTRADEFINITIONS UNIX_COMMAND "${ARGV4}")

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lib${DNAME}.rootmap ${CMAKE_CURRENT_BINARY_DIR}/G__${DNAME}.cxx ${CMAKE_CURRENT_BINARY_DIR}/G__${DNAME}_rdict.pcm
                       COMMAND
                         LD_LIBRARY_PATH=${ROOT_LIBDIR}:$ENV{LD_LIBRARY_PATH} ${ROOT_CINT}
                       ARGS
                         -f ${CMAKE_CURRENT_BINARY_DIR}/G__${DNAME}.cxx
                         -rmf ${CMAKE_CURRENT_BINARY_DIR}/lib${DNAME}.rootmap -rml lib${DNAME}
                         ${GLOBALDEFINITIONS} ${EXTRADEFINITIONS} ${INCLUDE_PATH} ${DHDRS} ${LDNAME}
                       DEPENDS
                         ${DHDRS} ${LDNAME} ${ROOT_CINT}
                       WORKING_DIRECTORY
                         ${CMAKE_CURRENT_BINARY_DIR}
                      )

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lib${DNAME}.rootmap" DESTINATION lib)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/G__${DNAME}_rdict.pcm" DESTINATION lib)
    
endmacro(generate_dictionary)
