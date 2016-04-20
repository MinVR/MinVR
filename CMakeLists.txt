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

if(NOT CMAKE_BUILD_TYPE)
  set( CMAKE_BUILD_TYPE Debug CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo
#MinSizeRel." FORCE)
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${${PROJECT_NAME}_SOURCE_DIR}/build/install" CACHE PATH "default install path" FORCE )
endif()


# Let's follow the convention of putting all of the libs in "build/lib" and all
# of the exes in "build/bin".  To distingusih between debugging, release, and
# other builds, we'll add a postfix to the name of the lib or exe that we generate.

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "")
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


if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux specific code
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC") 
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Apple-specific code
  set(CMAKE_CXX_FLAGS "-DOSX")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if (WIN32)
  # Windows-specific
endif (WIN32)



enable_testing()

set(vr_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(vr_config_cpp
  ${vr_src_dir}/config/VRDataIndex.cpp
  ${vr_src_dir}/config/VRDataQueue.cpp
  ${vr_src_dir}/config/VRDatum.cpp
  ${vr_src_dir}/config/VRDatumFactory.cpp
  ${vr_src_dir}/config/Cxml/attribute.cpp
  ${vr_src_dir}/config/Cxml/Cxml.cpp
  ${vr_src_dir}/config/Cxml/element.cpp
  ${vr_src_dir}/config/Cxml/utils.cpp
  ${vr_src_dir}/config/Cxml/XML.cpp
)

set(vr_config_h
  ${vr_src_dir}/config/VRCoreTypes.h
  ${vr_src_dir}/config/VRDataIndex.h
  ${vr_src_dir}/config/VRDataQueue.h
  ${vr_src_dir}/config/VRDatum.h
  ${vr_src_dir}/config/VRDatumFactory.h
  ${vr_src_dir}/config/Cxml/attribute.h
  ${vr_src_dir}/config/Cxml/Cxml.h
  ${vr_src_dir}/config/Cxml/element.h
  ${vr_src_dir}/config/Cxml/utils.h
  ${vr_src_dir}/config/Cxml/XML.h
)

set(vr_display_cpp
  ${vr_src_dir}/display/VRConsoleNode.cpp
  ${vr_src_dir}/display/VRDisplayNode.cpp
  ${vr_src_dir}/display/VRGraphicsWindowNode.cpp
  ${vr_src_dir}/display/VRGroupNode.cpp
  ${vr_src_dir}/display/VROffAxisProjectionNode.cpp
  ${vr_src_dir}/display/VRStereoNode.cpp
  ${vr_src_dir}/display/VRViewportNode.cpp
)

set(vr_display_h
  ${vr_src_dir}/display/VRConsoleNode.h
  ${vr_src_dir}/display/VRDisplayNode.h
  ${vr_src_dir}/display/VRGraphicsToolkit.h
  ${vr_src_dir}/display/VRGraphicsWindowNode.h
  ${vr_src_dir}/display/VRGroupNode.h
  ${vr_src_dir}/display/VROffAxisProjectionNode.h
  ${vr_src_dir}/display/VRStereoNode.h
  ${vr_src_dir}/display/VRViewportNode.h
  ${vr_src_dir}/display/VRWindowToolkit.h
)

set(vr_input_cpp
)

set(vr_input_h
  ${vr_src_dir}/input/VRInputDevice.h
)

set(vr_main_cpp
  ${vr_src_dir}/main/VRFactory.cpp
  ${vr_src_dir}/main/VRMain.cpp
)

set(vr_main_h
  ${vr_src_dir}/main/VREventHandler.h
  ${vr_src_dir}/main/VRFactory.h
  ${vr_src_dir}/main/VRMain.h
  ${vr_src_dir}/main/VRMainInterface.h
  ${vr_src_dir}/main/VRRenderHandler.h
)

set(vr_math_cpp
  ${vr_src_dir}/math/VRMath.cpp
  ${vr_src_dir}/math/VRRect.cpp
)

set(vr_math_h
  ${vr_src_dir}/math/VRMath.h
  ${vr_src_dir}/math/VRRect.h
)

set(vr_net_cpp
  ${vr_src_dir}/net/VRNetClient.cpp
  ${vr_src_dir}/net/VRNetInterface.cpp
  ${vr_src_dir}/net/VRNetServer.cpp
)

set(vr_net_h
  ${vr_src_dir}/net/VRNetClient.h
  ${vr_src_dir}/net/VRNetInterface.h
  ${vr_src_dir}/net/VRNetServer.h
)

set(vr_plugin_cpp
  ${vr_src_dir}/plugin/VRPluginManager.cpp
  ${vr_src_dir}/plugin/VRSharedLibrary.cpp
)

set(vr_plugin_h
  ${vr_src_dir}/plugin/VRPlugin.h
  ${vr_src_dir}/plugin/VRPluginManager.h
  ${vr_src_dir}/plugin/VRSharedLibrary.h
)

set(vr_sources 
  ${vr_config_cpp} ${vr_config_h}
  ${vr_display_cpp} ${vr_display_h}
  ${vr_input_h}
  ${vr_main_cpp} ${vr_main_h}
  ${vr_math_cpp} ${vr_math_h}
  ${vr_net_cpp} ${vr_net_h}
  ${vr_plugin_cpp} ${vr_plugin_h}
)

source_group("config\\Header Files" FILES ${vr_config_h})
source_group("config\\Source Files" FILES ${vr_config_cpp})
source_group("display\\Header Files" FILES ${vr_display_h})
source_group("display\\Source Files" FILES ${vr_display_cpp})
source_group("input\\Header Files" FILES ${vr_input_h})
source_group("input\\Source Files" FILES ${vr_input_cpp})
source_group("main\\Header Files" FILES ${vr_main_h})
source_group("main\\Source Files" FILES ${vr_main_cpp})
source_group("math\\Header Files" FILES ${vr_math_h})
source_group("math\\Source Files" FILES ${vr_math_cpp})
source_group("net\\Header Files" FILES ${vr_net_h})
source_group("net\\Source Files" FILES ${vr_net_cpp})
source_group("plugin\\Header Files" FILES ${vr_plugin_h})
source_group("plugin\\Source Files" FILES ${vr_plugin_cpp})


project(MinVR)

include_directories(
  ${vr_src_dir}
)

add_library(MinVR ${vr_sources})



# Recurse into the following subdirs to programs that compile
# separately from the MinVR Core lib

#add_subdirectory (plugins)
#add_subdirectory (examples)
#add_subdirectory (tests)



