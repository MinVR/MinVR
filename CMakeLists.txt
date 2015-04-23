# ================================================================================
# 
# This file is part of the MinVR Open Source Project, which is developed and
# maintained by the University of Minnesota's Interactive Visualization Lab.
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
# Copyright (c) 2015 Regents of the University of Minnesota
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
 
 
cmake_minimum_required (VERSION 2.8.2) 
project (MinVR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "cmake/modules")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/../install" CACHE PATH "default install path" FORCE )
endif()

set(MINVR_VERSION_MAJOR "2")
set(MINVR_VERSION_MINOR "0")
set(MINVR_VERSION_PATCH "0")
set(MINVR_VERSION_EXTRA "")
set(MINVR_VERSION "${MINVR_VERSION_MAJOR}.${MINVR_VERSION_MINOR}")
set(MINVR_VERSION_FULL "${MINVR_VERSION}.${MINVR_VERSION_PATCH}${MINVR_VERSION_EXTRA}")

#This way will auto detect all source files in the directory,
#while this will work, the preferred technique is to list out
#sourcefiles individually.  This will allow cmake to automatically
#rerun if a source file is added, which it will not do if you don't
#list the explicit source files.
#aux_source_directory(${CMAKE_SOURCE_DIR} SOURCEFILES})
#This is how you specific source file individually

set (HEADERFILES 
  core/include/TestClass.h
  ${EXTENDFILES}
)
set (SOURCEFILES 
  core/source/TestClass.cpp
)

set (EXTENDFILES
  extend/Plugin.h
  extend/PluginFramework.h
  extend/PluginInterface.h
  extend/PluginManager.h
  extend/SharedLibrary.h
)

set (HEADERFILES ${HEADERFILES} ${EXTENDFILES})

source_group("Header Files" FILES ${HEADERFILES})

############################################################
#Start of library detection
#Any libraries outside of your source should have a corresponding
#find_package method should be used here.
############################################################



############################################################
#End external library searching
############################################################

set (CMAKE_VERBOSE_MAKEFILE TRUE)

# Used for compiling in subdirectories of the project
include_directories (
  .
  ..
  core/include
)

############################################################
#OS Specific flags should go here
############################################################

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux specific code
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") 
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  # Windows specific code
endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

if (MSVC)
	# Windows specific code
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
	add_definitions(-DNOMINMAX)
endif (MSVC)


############################################################
# Add POSTFIX info
############################################################

set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")
set(CMAKE_MINSIZEREL_POSTFIX "s")

#set the build postfix extension according to the current configuration
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

#------------------------------------------
# Set output directories to lib, and bin
#------------------------------------------
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

############################################################
#Finally add all the targets here
############################################################

#You can also add subprojects that are in subdirectories
#These subdirectories must contain their own CMakeLists.txt

#add_subdirectory (some)

add_library ( ${PROJECT_NAME} ${HEADERFILES} ${SOURCEFILES} )

#------------------------------------------
# Install Target
#------------------------------------------

install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION bin)
         
install(FILES ${PROJECT_SOURCE_DIR}/LICENSE.txt DESTINATION ".")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/core/include/ DESTINATION "include")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/extend/ DESTINATION "include")