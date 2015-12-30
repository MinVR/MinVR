# ================================================================================
# 
# This file is part of the MinVR project.
# 
# File: CMakeLists.txt
# 
# Original Author(s) of this File:
# 	Dan Orban, 2015, University of Minnesota
# 
# Author(s) of Significant Updates/Modifications to the File:
# 	...
# 
# -----------------------------------------------------------------------------------
# Copyright (c) 2015 Regents of the University of Minnesota and Brown University
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
# 
# * Redistributions in binary form must reproduce the above copyright notice, this
#   list of conditions and the following disclaimer in the documentation and/or
#   other materials provided with the distribution.
# 
# * The name of the University of Minnesota, nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ================================================================================ 
 


# CMakeLists files in this project can refer to the root source directory of the 
# project as ${MINVR_SOURCE_DIR} and to the root binary directory of the project 
# as ${MINVR_BINARY_DIR}.

cmake_minimum_required (VERSION 2.8.2) 
project (MinVR)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${${PROJECT_NAME}_SOURCE_DIR}/build/install" CACHE PATH "default install path" FORCE )
endif()

# Use the root of the MinVR tree as an include dir, then include headers like this:
# #include <event/VREvent.h>

include_directories (${CMAKE_CURRENT_SOURCE_DIR})


# Let's follow the convention of putting all of the libs in "build/lib" and all
# of the exes in "build/bin".  To distingusih between debugging, release, and
# other builds, we'll add a postfix to the name of the lib or exe that we generate.

set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")
set(CMAKE_MINSIZEREL_POSTFIX "s")

if (CMAKE_BUILD_TYPE MATCHES "Release")
  set(CMAKE_BUILD_POSTFIX "${CMAKE_RELEASE_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
  set(CMAKE_BUILD_POSTFIX "${CMAKE_MINSIZEREL_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
  set(CMAKE_BUILD_POSTFIX "${CMAKE_RELWITHDEBINFO_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(CMAKE_BUILD_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
else()
  set(CMAKE_BUILD_POSTFIX "")
endif()

set(MinVR_BUILD_TYPE ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DMinVR_DEBUG")

make_directory(${CMAKE_BINARY_DIR}/lib)
make_directory(${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
  string (TOUPPER ${CONF} CONF)
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/bin)
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/lib)
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/lib)
endforeach(CONF CMAKE_CONFIGURATION_TYPES)


file (GLOB srcfiles ${CMAKE_CURRENT_SOURCE_DIR}/tests/net/*.cpp)
source_group ("tests" FILES ${srcfiles})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux specific code
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC") 
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Recurse into the following subdirs to build the core MinVR set of libraries:
# (Uncomment the lines below as we add .cpp files to those directories.)

add_subdirectory (config)
#add_subdirectory (data)
add_subdirectory (display)
add_subdirectory (event)
add_subdirectory (math)
add_subdirectory (net)
add_subdirectory (plugin)
add_subdirectory (main)
#add_subdirectory (apps)

# Now, build some programs, such as tests, that depend upon the MinVR libraries:

add_subdirectory (tests/net)
#add_subdirectory (tests/eventdata)
#add_subdirectory (tests/plugin)
add_subdirectory (tests/config)

add_subdirectory (contrib)
