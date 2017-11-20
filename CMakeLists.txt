# This file is part of the MinVR Open Source Project, which is developed and 
# maintained collaboratively by the University of Minnesota and Brown University.
#
# Copyright (c) 2015,2016,2017 Regents of the University of Minnesota and Brown University.
# This software is distributed under the BSD-3 Clause license, which can be found
# at: MinVR/LICENSE.txt.
# 
# Original Author(s) of this File: 
#   Dan Orban, 2015, University of Minnesota
#  
# Author(s) of Significant Updates/Modifications to the File:
#   Tom Sgouros, 2016, 2017, Brown University 
#      -- re-organized
#      -- improved status messages
#      -- cmake-repeat.sh
#   Dan Keefe, 2017, University of Minnesota 
#      -- made compiling itests optional since they have external dependencies
#      -- made all optional builds consistent, these are set using WITH_* flags for both plugins and itests
#      -- major rework of support for external projects (DEPENDENCIES_AUTOBUILD, DEPENDENCIES_INSTALL)
#      -- build/install plugins directly to lib/ rather than in subfolders
#      -- install headers to <prefix>/include/MinVR, extern projects use include/vrpn, include/TUIO, etc.
#      -- install config files to <prefix>/share/MinVR/config following filesystem standard:
#         http://refspecs.linuxfoundation.org/FHS_2.3/fhs-2.3.html
#      -- added installing cmake export files to <prefix>/share/MinVR/cmake
#      -- updated copyright headers in all CMakeLists.txt and .cmake files
#      -- updated README.md to walk through process of building and running a test graphics program
#   ...


project(MinVR_Project)
cmake_minimum_required (VERSION 3.3)

set(MINVR_VERSION 2.0)



list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Organize folder structure for Xcode, Visual Studio, etc.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(MessageMacros)


## ************************************************ WHAT ARE WE BUILDING?

h1("CONFIGURING BUILD TYPE")

if(NOT CMAKE_BUILD_TYPE)
  set( CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

message(STATUS "Building the '" ${CMAKE_BUILD_TYPE} "' version of MinVR.")
message(STATUS "If you want something different, use -DCMAKE_BUILD_TYPE=Debug, Release, RelWithDebInfo, MinSizeRel.")


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

message(STATUS "This build will append the postfix '" ${CMAKE_BUILD_POSTFIX} "' to any built libraries.")

# Add 'MinVR_DEBUG' to the symbols you can use in the code.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DMinVR_DEBUG")



## ************************************************ WHERE ARE WE PUTTING IT?

h1("CONFIGURING BUILD AND INSTALL LOCATIONS")

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

message(STATUS "Libraries will be built into " ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message(STATUS "Executables will be built into " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${${PROJECT_NAME}_SOURCE_DIR}/build/install" CACHE PATH "default install path" FORCE )
endif()

# Common rpath settings for shared libraries
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

message(STATUS "Installation will use this prefix: " ${CMAKE_INSTALL_PREFIX})



# You can use these symbol definitions in your code if you want to refer to a file
# location.  This is only really useful for the testing code, but knowing where the
# installed code is also plays a role in providing sensible default search paths.
add_definitions(-DINSTALLPATH="${CMAKE_INSTALL_PREFIX}")
add_definitions(-DMINVRTESTBATCHDIR="${CMAKE_SOURCE_DIR}/tests-batch")



## ************************************************ WHAT PLATFORM-SPECIFIC OPTIONS DO WE NEED?

h1("CONFIGURING PLATFORM-SPECIFIC BUILD OPTIONS")

message(STATUS "Building for " ${CMAKE_SYSTEM_NAME} ".")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux specific code
  add_definitions(-DLINUX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Apple-specific code
  add_definitions(-DOSX)
  set(CMAKE_MACOSX_RPATH ON)
endif()

if (WIN32)
  add_definitions(-DWIN32)
  # Windows-specific
endif()



## ************************************************ START WITH THE SRC FOR THE CORE LIBRARY

h1("CONFIGURING LIBMINVR")

h2("Configuring core library source.")
message(STATUS "Adding all subirectories of src to the build.")
set(vr_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories( ${vr_src_dir} )

add_subdirectory(src)


h2("Configuring .minvr config files.")
message(STATUS "Adding config/*.minvr to the build.")
add_subdirectory(config)


## ************************************************ ADD BATCH TESTS AND UTILS WITH NO DEPENDENCIES

h1("CONFIGURING TESTS AND UTILITY PROGRAMS")

h2("Configuring batch tests.")
message(STATUS "Adding all subirectories of tests-batch to the build.")
add_subdirectory(tests-batch)

h2("Configuring CMake Testing System.")
enable_testing()
message(STATUS "Testing enabled, try 'make test'.")

h2("Configuring utility programs.")
message(STATUS "Adding all subdirectories of utils to the build.")
add_subdirectory(utils)


## ************************************************ OPTIONALLY ADD PLUGINS AND ITESTS WITH EXTERNAL DEPENDENCIES 

h1("CONFIGURING OPTIONAL PLUGINS")


option(DEPENDENCIES_AUTOBUILD "If enabled, dependencies that cmake cannot find on the system will be downloaded to the `external' directory and built and installed as external projects during the MinVR build and install processes." ON)

if (DEPENDENCIES_AUTOBUILD)
    message(STATUS "DEPENDENCIES_AUTOBUILD is ON.")
else()
    message(STATUS "DEPENDENCIES_AUTOBUILD is OFF.")
endif()


option(DEPENDENCIES_INSTALL "If enabled, autobuilt dependencies will be installed to the same CMAKE_INSTALL_PREFIX as MinVR during the MinVR install step." ON)

if (DEPENDENCIES_INSTALL)
    message(STATUS "DEPENDENCIES_INSTALL is ON.")
else()
    message(STATUS "DEPENDENCIES_INSTALL is OFF.")
endif()


set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)
file(MAKE_DIRECTORY ${EXTERNAL_DIR})



#------------------ Plugins ------------------ 

macro(minvr_optional_adddir optionflag description dependsnote dir)
  h2("Configuring ${description}. ${dependsnote}")
  option(${optionflag} "Include ${description} in the build.  ${dependsnote}")
  if (${optionflag})
    message(STATUS "ON: Will build and install ${description}.")
    add_subdirectory(${dir})
  else()
    message(STATUS "OFF: NOT building ${description}.  Choose it with '-D ${optionflag}=ON'.")
  endif()
endmacro()

macro(minvr_optional optionflag description dependsnote)
  h2("Configuring ${description}. ${dependsnote}")
  option(${optionflag} "Include ${description} in the build.  ${dependsnote}")
  if (${optionflag})
    message(STATUS "ON: Will build and install ${description}.")
  else()
    message(STATUS "OFF: NOT building ${description}.  Choose it with '-D ${optionflag}=ON'.")
  endif()
endmacro()



#---- OpenGL Plugin ----
minvr_optional_adddir(
  WITH_PLUGIN_OPENGL
  "OpenGL Plugin"
  "(Requires OpenGL to be pre-installed and requires GLEW on Windows only)"
  "plugins/OpenGL"
)


#---- GLFW Plugin ----
minvr_optional_adddir(
  WITH_PLUGIN_GLFW
  "GLFW Plugin"
  "(Requires GLFW and OpenGL, and GLEW on Windows only)"
  "plugins/GLFW"
)


#---- VRPN ----
minvr_optional_adddir(
  WITH_PLUGIN_VRPN
  "VRPN Plugin"
  "(Requires the VRPN library)"
  "plugins/VRPN"
)


#---- Python ----
minvr_optional_adddir(
  WITH_PLUGIN_PYTHON
  "Python Bindings Plugin"
  "(Requires Python version ????)"
  "plugins/Python"
)


#---- Threading ----
minvr_optional_adddir(
  WITH_PLUGIN_THREADING
  "Threading Plugin"
  "(Requires a C++-11 capable compiler)"
  "plugins/Threading"
)


#---- TUIO ----
minvr_optional_adddir(
  WITH_PLUGIN_TUIO
  "TUIO Plugin"
  "(Requires the TUIO library)"
  "plugins/TUIO"
)


#---- OpenVR ----
minvr_optional_adddir(
  WITH_PLUGIN_OPENVR
  "OpenVR Plugin"
  "(Requires the OpenVR library)"
  "plugins/OpenVR"
)


#---- FreeGlut ----
minvr_optional_adddir(
  WITH_PLUGIN_FREEGLUT
  "FREEGLUT Plugin"
  "(Requires the FreeGlut library)"
  "plugins/FREEGLUT"
)


#---- Scalable ----
minvr_optional_adddir(
  WITH_PLUGIN_SCALABLE
  "Scalable Plugin"
  "(Requires proprietary software from scalabledisplay.com)"
  "plugins/Scalable"
)


# ---- Plugins that Provide a Window Toolkit ----
# Save a list of plugins that provide a window toolkit so that we can error out
# if building a graphics program and no window toolkit is available.

list(APPEND WINTOOLKIT_PLUGINS WITH_PLUGIN_GLFW WITH_PLUGIN_FREEGLUT)
set(HAVE_WINTOOLKIT False)

foreach(P ${WINTOOLKIT_PLUGINS})
  if (${P})
    set(HAVE_WINTOOLKIT True)
  endif()
endforeach(P)



#------------------ Interactive Tests ------------------ 

h1("CONFIGURING OPTIONAL INTERACTIVE TESTS")

if (DEPENDENCIES_AUTOBUILD)
    message(STATUS "DEPENDENCIES_AUTOBUILD is ON.")
else()
    message(STATUS "DEPENDENCIES_AUTOBUILD is OFF.")
endif()


option(DEPENDENCIES_INSTALL "If enabled, autobuilt dependencies will be installed to the same CMAKE_INSTALL_PREFIX as MinVR during the MinVR install step." ON)

if (DEPENDENCIES_INSTALL)
    message(STATUS "DEPENDENCIES_INSTALL is ON.")
else()
    message(STATUS "DEPENDENCIES_INSTALL is OFF.")
endif()



minvr_optional_adddir(
  WITH_ITEST_OPENGL_FIXEDFUNCPIPELINE 
  "itest-opengl-fixedfuncpipeline"
  "(Requires the OpenGL Plugin and a WindowToolkit plugin (e.g., GLFW Plugin))" 
  "tests-interactive/opengl-fixedfuncpipeline"
)
if (WITH_ITEST_OPENGL_FIXEDFUNCPIPELINE AND NOT ${WITH_PLUGIN_OPENGL})
  message(FATAL_ERROR "WITH_ITEST_OPENGL_FIXEDFUNCPIPELINE requires that you also build WITH_PLUGIN_OPENGL.")
endif()
if (WITH_ITEST_OPENGL_FIXEDFUNCPIPELINE AND NOT ${HAVE_WINTOOLKIT})
  message(FATAL_ERROR "WITH_ITEST_OPENGL_FIXEDFUNCPIPELINE requires that you also build a plugin that provides a window toolkit.  You need to add one of the following: ${WINTOOLKIT_PLUGINS}.")
endif()


minvr_optional_adddir(
  WITH_ITEST_OPENGL_MULTITHREADED 
  "itest-opengl-multithreaded"
  "(Requires the OpenGL Plugin, the Threading Plugin, and a WindowToolkit plugin (e.g., GLFW Plugin))" 
  "tests-interactive/opengl-multithreaded"
)
if (WITH_ITEST_OPENGL_MULTITHREADED AND NOT ${WITH_PLUGIN_OPENGL})
  message(FATAL_ERROR "WITH_ITEST_OPENGL_MULTITHREADED requires that you also build WITH_PLUGIN_OPENGL.")
endif()
if ((WITH_ITEST_OPENGL_MULTITHREADED) AND (NOT ${WITH_PLUGIN_THREADING}))
  message(FATAL_ERROR "WITH_ITEST_OPENGL_MULTITHREADED requires that you also build WITH_PLUGIN_THREADING.")
endif()
if (WITH_ITEST_OPENGL_MULTITHREADED AND NOT ${HAVE_WINTOOLKIT})
  message(FATAL_ERROR "WITH_ITEST_OPENGL_MULTITHREADED requires that you also build a plugin that provides a window toolkit.  You need to add one of the following: ${WINTOOLKIT_PLUGINS}.")
endif()



minvr_optional_adddir(
  WITH_ITEST_OPENGL_SHADERPIPELINE 
  "itest-opengl-shaderpipeline" 
  "(Requires the OpenGL Plugin and a WindowToolkit plugin (e.g., GLFW Plugin))" 
  "tests-interactive/opengl-shaderpipeline"
)
if (WITH_ITEST_OPENGL_SHADERPIPELINE AND NOT ${WITH_PLUGIN_OPENGL})
  message(FATAL_ERROR "WITH_ITEST_OPENGL_SHADERPIPELINE requires that you also build WITH_PLUGIN_OPENGL.")
endif()
if (WITH_ITEST_OPENGL_SHADERPIPELINE AND NOT ${HAVE_WINTOOLKIT})
  message(FATAL_ERROR "WITH_ITEST_OPENGL_SHADERPIPELINE requires that you also build a plugin that provides a window toolkit.  You need to add one of the following: ${WINTOOLKIT_PLUGINS}.")
endif()

minvr_optional_adddir(
  WITH_ITEST_OPENGL_SHADERPIPELINE_WITH_API 
  "itest-opengl-shaderpipeline-with-api"
  "(Requires the OpenGL Plugin and a WindowToolkit plugin (e.g., GLFW Plugin))" 
  "tests-interactive/opengl-shaderpipeline-with-api"
)
if (WITH_ITEST_OPENGL_SHADERPIPELINE_WITH_API AND NOT ${WITH_PLUGIN_OPENGL})
  message(FATAL_ERROR "WITH_ITEST_OPENGL_SHADERPIPELINE requires that you also build WITH_PLUGIN_OPENGL.")
endif()
if (WITH_ITEST_OPENGL_SHADERPIPELINE_WITH_API AND NOT ${HAVE_WINTOOLKIT})
  message(FATAL_ERROR "WITH_ITEST_OPENGL_SHADERPIPELINE_WITH_API requires that you also build a plugin that provides a window toolkit.  You need to add one of the following: ${WINTOOLKIT_PLUGINS}.")
endif()

# There's not actually anything to build here, right?
#minvr_optional_adddir(
#  WITH_ITEST_PYTHONBINDINGS_OPENGL 
#  "itest-pythonbindings-opengl"
#  "(Requires the OpenGL Plugin, the Python Bindings Plugin, and a WindowToolkit plugin (e.g., GLFW Plugin))" 
#  "tests-interactive/pythonbindings-opengl" 
#)





## ************************************************ WHAT ABOUT DOCUMENTATION?

h1("CONFIGURING DOCUMENTATION")

minvr_optional(
  WITH_DOCUMENTATION 
  "MinVR API Documentation Web Pages"
  "(Requires Doxygen)" 
)
if (WITH_DOCUMENTATION)

  find_package(Doxygen)

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

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${CMAKE_INSTALL_PREFIX}/share/MinVR/doc)
endif()


## ************************************************ CREATE A CMAKE CONFIG FILE TO HELP OTHER PROJECTS

h1("WRITING CMAKE CONFIG PACKAGE")

# https://cmake.org/cmake/help/git-master/manual/cmake-packages.7.html#creating-packages

set_property(TARGET MinVR PROPERTY VERSION ${MINVR_VERSION})


include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/MinVR/MinVRConfigVersion.cmake"
  VERSION ${MINVR_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT MinVRTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/MinVR/MinVRTargets.cmake"
  NAMESPACE MinVR::
)

configure_file(cmake/MinVRConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/MinVR/MinVRConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/MinVR)
install(EXPORT MinVRTargets
  FILE
    MinVRTargets.cmake
  NAMESPACE
    MinVR::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/MinVR/MinVRConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/MinVR/MinVRConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)



## ************************************************ HOW ARE WE BUILDING?

h1("SUMMARY INFORMATION")

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

message(STATUS "CMAKE invocation stored in ${CMAKE_BINARY_DIR}/cmake-repeat.sh.\n   Feel free to edit, then execute that file.")
