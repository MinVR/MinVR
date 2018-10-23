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
#      -- major rework of support for external projects (AUTOBUILD_DEPENDENCIES)
#      -- build/install plugins directly to lib/ rather than in subfolders
#      -- install headers to <prefix>/include/MinVR, extern projects use include/vrpn, include/TUIO, etc.
#      -- install config files to <prefix>/share/MinVR/config following filesystem standard:
#         http://refspecs.linuxfoundation.org/FHS_2.3/fhs-2.3.html
#      -- updated copyright headers in all CMakeLists.txt and .cmake files
#      -- updated README.md to walk through process of building and running a test graphics program
#      -- added installing a CMake Config Package to <prefix>/lib/cmake/MinVR and added comments
#         documenting this feature, which is really hard to understand from the cmake docs.
#      -- updated CMakeLists.txt files to use new style target_*() cmake functions
#      -- changed MinVR install dirs to include a version number so that multiple versions can be
#         installed on the same system without conflicts
#      -- installed headers for the plugins
#      -- added plugins to the MinVRConfig.cmake package using target_include_dirs() with different
#         settings for build and install so the package is relocatable
#      -- updated to requiring cmake 3.9+ in order to pull in cmake's updated FindOpenGL and FindGLEW,
#         which define new-style cmake imported targets.
#      -- added using cmake configure_file() command with VRConfig.h.in to pass MinVR version numbers
#         and build and install dirs to the MinVR library.  Updated searching for config files and
#         plugins to use these paths.
#      -- changed to building dependencies during cmake's configure process rather than the build process.
#         This is different than the way cmake's ExternalProject_add() command works, but I think it is
#         preferable because there is a chicken and the egg problem with building dependencies.  If the
#         depends do not exist at the time MinVR is configured, then it is impossible to load them in
#         as imported targets, but with ExternalProject_add, the dependencies are not built until after
#         MinVR is configured.  The proposed solution here is to download, build, and install all 
#         dependencies during the configure step if find_package() is unable to find them already installed
#         on the system.  Then, once they are installed, we can simply re-run find_package() and continue
#         on with configuring MinVR as if they were found in the first place.  One other advantage of this
#         approach is that the way MinVR links with external libs is exactly the same (same variables are
#         set, etc.) regardless of whether the external lib is found with find_package() or whether we build
#         it ourselves.  In our prior approach this was not the case, so we were maintaining two approaches
#         for each dependency.
#      -- not part of this main CMakeLists.txt file, but also added an examples directory with a 
#         CavePainting-Lite example project.  It is a completely separate cmake build system that
#         demonstrates: 1) how to link with MinVR using the new MinVRConfig.cmake config package that gets
#         installed with MinVR.  2) how to access tracker and button events -- basically an example of a
#         real VR program rather than just a test to verify rendering works, as in the itest-* apps.       
#   ...




# MinVR depends on cmake 3.9+ for its more advanced FindOpenGL.cmake module
cmake_minimum_required (VERSION 3.9)

cmake_policy(SET CMP0048 NEW)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

message(STATUS ${CMAKE_MODULE_PATH})

# Version is <major>.<minor>.<patch>
project(MinVR_Toolkit VERSION 2.0.0)


# This will be appended to the install destinations so that multiple versions of MinVR
# can be safely installed on the same system.  This intentionally does not include a
# patch number because patches should be small changes that do not change the API.
set(MINVR_VERSION_STR -${MinVR_Toolkit_VERSION_MAJOR}.${MinVR_Toolkit_VERSION_MINOR})


set(MINVR_PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})


# Organize folder structure for Xcode, Visual Studio, etc.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include(MessageMacros)


include(AutoBuild)
AutoBuild_init()

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

# Since we add postfixes to distinguish between debug, release, etc., we can just put all
# of the libs in "build/lib" and all of the exes in "build/bin".

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

h2("Configuring build directories.")
message(STATUS "Utilities, tests, other binaries: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Libraries: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")



if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

# When we autobuild dependencies, we install them in CMAKE_INSTALL_PREFIX, and then look for them
# there using find_package().  So, this makes sure the install prefix is in the path that 
# find_package() searches.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})


# These are relative to ${CMAKE_INSTALL_PREFIX}
set(INSTALL_BIN_DEST           bin)
set(INSTALL_LIB_DEST           lib/MinVR${MINVR_VERSION_STR})
set(INSTALL_INCLUDE_DEST       include/MinVR${MINVR_VERSION_STR})
set(INSTALL_PLUGINLIB_DEST     lib/MinVR${MINVR_VERSION_STR})
set(INSTALL_PLUGINDLL_DEST     bin/MinVR${MINVR_VERSION_STR})
set(INSTALL_PLUGININCLUDE_DEST include/MinVR${MINVR_VERSION_STR})
set(INSTALL_CONFIGFILES_DEST   share/MinVR${MINVR_VERSION_STR}/config)
set(INSTALL_DOC_DEST           share/MinVR${MINVR_VERSION_STR}/doc)
set(INSTALL_CMAKECONFIG_DEST   lib/cmake/MinVR${MINVR_VERSION_STR})


h2("Configuring installation directories.")
message(STATUS "Utilities, tests, other binaries: ${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN_DEST}")
message(STATUS "MinVR library: ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DEST}")
message(STATUS "MinVR header files: ${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DEST}")
message(STATUS "MinVR config files for pre-defined VR Setups: ${CMAKE_INSTALL_PREFIX}/${INSTALL_CONFIGFILES_DEST}")
message(STATUS "CMake Config Package for including MinVR in other projects: ${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKECONFIG_DEST}")
message(STATUS "MinVR plugin shared libraries: ${CMAKE_INSTALL_PREFIX}/${INSTALL_PLUGINLIB_DEST}")
message(STATUS "MinVR plugin .dlls (windows only): ${CMAKE_INSTALL_PREFIX}/${INSTALL_PLUGINDLL_DEST}")
message(STATUS "MinVR plugin header files: ${CMAKE_INSTALL_PREFIX}/${INSTALL_PLUGININCLUDE_DEST}/<PLUGINNAME>")


# Common rpath settings for shared libraries
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 


h2("Creating VRConfig.h.")
# Configure a header file to pass some of the CMake settings to the source code
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/VRConfig.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/VRConfig.h
)




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
  # Windows-specific
  add_definitions(-DWIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()



## ************************************************ START WITH THE SRC FOR THE CORE LIBRARY

h1("CONFIGURING LIBMINVR")

h2("Configuring core library source.")
message(STATUS "Adding all subirectories of src to the build.")


# Intentionally using include() rather than add_subdirectory() so that the MinVR target
# is defined in the scope of this CMakeLists.txt file.  That way, sudirs that depend
# upon it can list it as a dependency.
include(src/CMakeLists.txt)


h2("Configuring .minvr config files.")
message(STATUS "Adding config/*.minvr to the build.")
add_subdirectory(config)


## ************************************************ ADD BATCH TESTS AND UTILS WITH NO DEPENDENCIES

h1("CONFIGURING TESTS AND UTILITY PROGRAMS")



h2("Configuring CMake Testing System.")
enable_testing()
message(STATUS "Testing enabled, try 'make test'.")

# Update the testing dashboard at http://my.cdash.org/index.php?project=MinVR
include(CTest)

h2("Configuring batch tests.")
message(STATUS "Adding all subirectories of tests-batch to the build.")
add_subdirectory(tests-batch)

h2("Configuring utility programs.")
message(STATUS "Adding all subdirectories of utils to the build.")
add_subdirectory(utils)


## ************************************************ OPTIONALLY ADD PLUGINS AND ITESTS WITH EXTERNAL DEPENDENCIES 

h1("CONFIGURING OPTIONAL PLUGINS")


option(AUTOBUILD_DEPENDENCIES "If enabled, dependencies that cmake cannot find on the system will be downloaded to the `external' directory and built and installed to CMAKE_INSTALL_PREFIX during the MinVR configure process as a precursor to the MinVR build process." ON)

if (AUTOBUILD_DEPENDENCIES)
    message(STATUS "AUTOBUILD_DEPENDENCIES is ON.")
else()
    message(STATUS "AUTOBUILD_DEPENDENCIES is OFF.")
endif()

set(EXTERNAL_DIR_NAME external)
set(EXTERNAL_DIR ${MINVR_PROJECT_ROOT}/${EXTERNAL_DIR_NAME})
file(MAKE_DIRECTORY ${EXTERNAL_DIR})



#------------------ Plugins ------------------ 

macro(minvr_optional_adddir optionflag description dependsnote dir)
  h2("Configuring ${description}. ${dependsnote}")
  option(${optionflag} "If enabled, includes ${description} in the build.  ${dependsnote}")
  if (${optionflag})
    message(STATUS "ON: Will build and install ${description}.")
    add_subdirectory(${dir})
  else()
    message(STATUS "OFF: NOT building ${description}.  Choose it with '-D ${optionflag}=ON'.")
  endif()
endmacro()

macro(minvr_optional optionflag description dependsnote)
  h2("Configuring ${description}. ${dependsnote}")
  option(${optionflag} "If enabled, includes ${description} in the build.  ${dependsnote}")
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

if (AUTOBUILD_DEPENDENCIES)
    message(STATUS "AUTOBUILD_DEPENDENCIES is ON.")
else()
    message(STATUS "AUTOBUILD_DEPENDENCIES is OFF.")
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

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html 
      DESTINATION ${INSTALL_DOC_DEST}
      COMPONENT Docs
      OPTIONAL
  )
endif()


## ************************************************ CREATE A CMAKE CONFIG FILE TO HELP OTHER PROJECTS

h1("WRITING CMAKE CONFIG PACKAGE")

# The documentation for setting this up is so bad.  These pages all helped.  My current
# understanding (dfk) is summarized below to help others.
# https://cmake.org/cmake/help/git-master/manual/cmake-packages.7.html#creating-packages
# https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements
# http://foonathan.net/blog/2016/03/03/cmake-install.html
# https://archive.fosdem.org/2013/schedule/event/moderncmake/attachments/slides/258/export/events/attachments/moderncmake/slides/258/cmake_fosdem_2013.pdf

# CMake Package Config in a Nutshell:

# CMake config packages are the newer, recommended way of distributing projects built with CMake.
# They replace the older FindModule.cmake approach for including dependencies in a cmake project, 
# but not everyone has yet switched over to the new approach.  Both styles can work together, and
# this seems to often be required because many libraries are not yet distributing a *Config.cmake
# file with their install tree.


# 1. The Difference Between "Find Modules" (Find*.cmake) and "Config Packages" (*Config.cmake)

# Whereas, Find*.cmake files are either installed on the system or written by downstream projects.
# *Config.cmake files are created and installed by the upstream projects (e.g., we want generate
# MinVRConfig.cmake and install it whenever the MinVR project in installed).

# When you use a Find Module, it defines several variables (e.g., OPENGL_INCLUDE_DIR, OPENGL_LIBRARIES)
# that you must then be sure to add to the include dirs and libs to link with your project.
# Config Packages are different.  Instead, they "export" the targets defined in the original project
# (e.g., the target MinVR is used in this file to build libMinVR.a) so that the client project
# may refer to these targets directly.  This is pretty cool because we can define the MinVR target
# in such a way that cmake even knows which header files are associated with it, and will add the
# required include flags whenever the MinVR target is used.


# 2. How to use Config Packages in a client CMakeLists.txt 

# With the config package appraoch, client projects only need to include two lines to use our library:

# find_package(MinVR REQUIRED)
# ... then, after add_executable(${PROJECT_NAME}) or add_library(${PROJECT_NAME})
# target_link_libraries(${PROJECT_NAME} PUBLIC MinVR)


# 3. Finding *Config.cmake files

# Config packages are files of the form MyProjNameConfig.cmake (or *-config.cmake).  Each library
# provides and installs its own config package.  So, here we will create one to install with MinVR.
# If created correctly, these packages can be relocatable, i.e., not specific to the install path
# used on a particular machine.  When you call find_package(...) cmake looks for packages in a
# variety of locations (e.g., /usr/local/lib/cmake/*).  

# Since config packages are supposed to be written by and installed by the upstream dependencies,
# my understanding is that you should not attempt to write you own version of a config package if
# you encounter a project that is not (yet) using the config package appraoch.  Instead, use the
# older Find Module appraoch.  If CMake ships with a Find*.cmake file for that project, then use 
# it.  If not, then you can write your own.  Since making this switch, many of the newer
# Find*.cmake scripts that ship with CMake have been updated to support imported targets. They
# are still called something like FindOpenGL.cmake, but in addition to just setting vars like
# OPENGL_INCLUDE_DIR, they also define targets, such as OpenGL::GL.  We use these when we can
# because they are more robust.  When you do target_link_libraries(myproj PULBIC OpenGL::GL)
# cmake knows to not only link with the the opengl library but also to include its headers and
# link with all dependencies.


# This tells cmake to generate a MinVRTargets.cmake file that works for importing all targets
# associated with the MinVR export group from the install tree.  This file is where all the
# important cmake magic happens. It is auto-generated.  This call also installs the file to 
# the appropriate location.
install(EXPORT MinVRTargets
  NAMESPACE MinVR::
  FILE MinVRTargets.cmake
  DESTINATION ${INSTALL_CMAKECONFIG_DEST}
)

# This is a very small wrapper around the MinVRTargets.cmake file.  It's possible to use this
# configure_file() command to replace variables (e.g., special defines, install paths) in the 
# file, but we do not currently need that functionality.
configure_file(
  cmake/MinVRConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/MinVRConfig.cmake"
  COPYONLY
)

# This auto-generates a standard CMake config version file. Using this, client projects can
# check to see if a specific version of MinVR is installed on the system.
include(CMakePackageConfigHelpers)
set_property(TARGET MinVR PROPERTY VERSION ${MinVR_Toolkit_VERSION})
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/MinVRConfigVersion.cmake"
  VERSION ${MinVR_Toolkit_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# The two Config files also get installed
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/MinVRConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/MinVRConfigVersion.cmake"
  DESTINATION
    ${INSTALL_CMAKECONFIG_DEST}
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


## ************************************************ 

AutoBuild_check_status()



