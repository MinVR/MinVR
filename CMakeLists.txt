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
# 	MinVR Core Team
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

project(MinVR_Project)
cmake_minimum_required (VERSION 2.8.2)

# This policy has to do with how to compare variable values and strings.
# The change from old behavior to new happened in 3.1, so in order to
# support earlier and later version of cmake, we set a policy here.
cmake_policy(SET CMP0054 NEW)


# CMakeLists files in this project can refer to the root source directory of the
# project as ${MINVR_SOURCE_DIR} and to the root binary directory of the project
# as ${MINVR_BINARY_DIR}.


## ************************************************ WHAT ARE WE BUILDING?

if(NOT CMAKE_BUILD_TYPE)
  set( CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

message(STATUS "Building the '" ${CMAKE_BUILD_TYPE} "' version of MinVR.")
message(STATUS "If you want something different, use -DCMAKE_BUILD_TYPE=Debug, Release, etc.")

# To distinguish between debugging, release, and other builds, we'll
# add a postfix to the name of the lib or exe that we generate.

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()
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

if (CMAKE_BUILD_POSTFIX)
  message(STATUS "Appending '" ${CMAKE_BUILD_POSTFIX} "' to the built library file name.")
endif()
  
# Add 'MinVR_DEBUG' to the symbols you can use in the code.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DMinVR_DEBUG")


## ************************************************ WHERE ARE WE PUTTING IT?

# Let's follow the convention of putting all of the libs in "build/lib" and all
# of the exes in "build/bin". 

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

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${${PROJECT_NAME}_SOURCE_DIR}/build/install" CACHE PATH "default install path" FORCE )
endif()

message(STATUS "Libraries and include files will be installed to " ${CMAKE_INSTALL_PREFIX})

# Organize folder structure for Xcode, Visual Studio, etc.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# You can use these symbol definitions in your code if you want to refer to a file
# location.  This is only really useful for the testing code, but knowing where the
# installed code is also plays a role in providing sensible default search paths.
add_definitions(-DINSTALLPATH="${CMAKE_INSTALL_PREFIX}")
add_definitions(-DMINVRTESTBATCHDIR="${CMAKE_SOURCE_DIR}/tests-batch")


## ************************************************ HOW ARE WE BUILDING IT?

message(STATUS "Building for " ${CMAKE_SYSTEM_NAME} ".")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux specific code
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Apple-specific code
  set(CMAKE_CXX_FLAGS "-DOSX")
  set(CMAKE_MACOSX_RPATH false)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if (WIN32)
  # Windows-specific
endif (WIN32)

## ************************************************ WHAT ABOUT DOCUMENTATION?

# Add a target to generate API documentation with Doxygen
find_package(Doxygen)

option(BUILD_DOCUMENTATION "If enabled, creates and installs the HTML based API documentation (requires Doxygen).")

if(BUILD_DOCUMENTATION)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation.")
  endif()

  message(STATUS "Configured to build documentation.  Use 'make doc'.")

  # Process the config file and put it in the build directory.
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/MinVR.doxygen.in)
  set(doxyfile ${CMAKE_BINARY_DIR}/MinVR.doxygen)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)

  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)

else()
  message(STATUS "NOT building MinVR library documentation.\n     Use 'cmake -DBUILD_DOCUMENTATION=on' to build.")

endif()

enable_testing()
message(STATUS "Testing enabled, try 'make test'.")


set(vr_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories( ${vr_src_dir} )

message(STATUS "Examining source.")
add_subdirectory(src)
message(STATUS "Configuring plugins.")
add_subdirectory(plugins)
message(STATUS "Configuring batch tests.")
add_subdirectory(tests-batch)
message(STATUS "Configuring interactive tests.")
add_subdirectory(tests-interactive)
message(STATUS "Configuring utility applications.")
add_subdirectory(utils)


## ************************************************ HOW ARE WE BUILDING?

# The following sorts through all the cmake variables, and stores the ones
# whose help strings include the magic string "If enabled", plus the cmake
# build type (Debug, Release, etc.) and install directory.  It then stores a
# simulacrum of the cmake invocation in a shell script, so you can run it
# again if you like.
#
# This also means that if you add options, make their help strings include
# the magic string.

# Get a list of all the cmake variables.
get_cmake_property(CACHE_VARS CACHE_VARIABLES)

# Start our invocation string.
set(CMAKE_INVOCATION "cmake ${CMAKE_SOURCE_DIR} ")

# Loop through all the variables, and if they match the magic string, or if
# they are the build type, add them to the invocation string.
foreach(CACHE_VAR ${CACHE_VARS})
  get_property(CACHE_VAR_HELPSTRING CACHE ${CACHE_VAR} PROPERTY HELPSTRING)
  if ("${CACHE_VAR_HELPSTRING}" MATCHES "If enabled" OR
      # There are also a few commonly-used cmake variables worth recording.
      "${CACHE_VAR}" STREQUAL "CMAKE_BUILD_TYPE" OR
      "${CACHE_VAR}" STREQUAL "CMAKE_INSTALL_PREFIX")
    get_property(CACHE_VAR_TYPE CACHE ${CACHE_VAR} PROPERTY TYPE)
    if(CACHE_VAR_TYPE STREQUAL "UNINITIALIZED")
      set(CACHE_VAR_TYPE)
    else()
      set(CACHE_VAR_TYPE :${CACHE_VAR_TYPE})
    endif()
    set(CMAKE_INVOCATION "${CMAKE_INVOCATION} -D${CACHE_VAR}${CACHE_VAR_TYPE}=\"${${CACHE_VAR}}\"")
  endif()
endforeach()
file(WRITE ${CMAKE_BINARY_DIR}/cmake-repeat.sh "#!/usr/bin/env bash\n# Feel free to edit this file before executing it. The file will be\n# overwritten with each cmake invocation, but the saved version will\n# reflect your changes.\n${CMAKE_INVOCATION}\n")

message(STATUS "CMAKE invocation stored in ${CMAKE_BINARY_DIR}/cmake-repeat.sh.\n     Feel free to edit, then execute that file.")
