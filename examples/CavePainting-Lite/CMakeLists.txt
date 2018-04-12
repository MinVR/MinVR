# This file is part of the CavePainting-Lite project.  
# See LICENSE.txt for full authors, copyright, and license info.
#
# Original Author(s) of this File: 
#   Daniel Keefe, 2017, University of Minnesota
#  
# Author(s) of Significant Updates/Modifications to the File:
#   ... 



# You are encouraged to copy this example, move it outside of the MinVR directory, and use
# it as a starting point for your VR project.  When you do this, you'll have to edit the
# following line as needed to point to the MinVR install prefix used on your system.

# !!!!!!!!!!!!! EDIT THE FOLLOWING LINE AS NEEDED !!!!!!!!!!!!! 
list(APPEND CMAKE_PREFIX_PATH ../../build/install)



# Using 3.9 to get a modern version of FindOpenGL.cmake
cmake_minimum_required (VERSION 3.9) 

# Dependencies that are auto-downloaded, built, and installed for you will go here.  So, you 
# may use this even if not planning to install this particular project on your system.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

list(APPEND CMAKE_MODULE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_INSTALL_PREFIX})
list(INSERT CMAKE_PREFIX_PATH 0 ${CMAKE_INSTALL_PREFIX})

include(AutoBuild)
AutoBuild_init()


#---------------------- Source for this Project ----------------------

project(CavePainting-Lite)

message(STATUS "==== BUILDING ${PROJECT_NAME}")
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")



set(source_files
  main.cpp
  QuickShapes.cpp
  UIExample.cpp
)

set(header_files
  QuickShapes.h
  UIExample.h
)

set(extra_files
  README.md
)

set_source_files_properties(${extra_files} PROPERTIES HEADER_FILE_ONLY TRUE)



#---------------------- Define the Target ----------------------


add_executable(${PROJECT_NAME} ${source_files} ${header_files} ${extra_files})



#---------------------- Find & Add Dependencies ----------------------

set(EXTERNAL_DIR_NAME external)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${EXTERNAL_DIR_NAME})
set(EXTERNAL_CMAKE_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# MinVR (linked with an imported cmake target so no need to specify include dirs)
find_package(MinVR REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC MinVR::MinVR)


# OpenGL
include(AutoBuildOpenGL)
AutoBuild_use_package_OpenGL(${PROJECT_NAME} PUBLIC)


# GLM
AutoBuild_use_package(glm . ${PROJECT_NAME} PUBLIC glm
    DOWNLOAD_ARGS  GIT_REPOSITORY "https://github.com/g-truc/glm.git" GIT_TAG master
)


# GLEW
if (NOT APPLE)
    include(AutoBuildGLEW)
    AutoBuild_use_package_GLEW(${PROJECT_NAME} PUBLIC)
endif()



#---------------------- Enable Testing - Needed for Nightly Builds of MinVR ----------------------
enable_testing()
include(CTest)



AutoBuild_check_status()
