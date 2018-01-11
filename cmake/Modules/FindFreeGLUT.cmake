# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec <kapecp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
#	Note:
#		Searching headers and libraries is very simple and is NOT as powerful as scripts
#		distributed with CMake, because LuaDist defines directories to search for.
#		Everyone is encouraged to contact the author with improvements. Maybe this file
#		becomes part of CMake distribution sometimes.

# - Find FreeGLUT
# Find the native FreeGLUT headers and libraries.
#
#  FREEGLUT_INCLUDE_DIRS - where to find freeglut.h, etc.
#  FREEGLUT_LIBRARIES    - List of libraries when using FreeGLUT.
#  FREEGLUT_FOUND        - True if FreeGLUT found.

# Hack: On OSX, this makes sure *not* to use the Glut files that come
# with the system.  These appear not to be the same freeglut that we
# are counting on.
set(CMAKE_FIND_FRAMEWORK "NEVER")

# Look for the header file.
FIND_PATH(FREEGLUT_INCLUDE_DIR 
  NAMES 
    GL/freeglut.h
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/include 
    $ENV{FREEGLUT_ROOT}/include 
    /usr/local/include
    ENV CPATH # For OSCAR modules at Brown/CCV
    /usr/local/Cellar/freeglut/2.8.1/include # This is how it comes with Brew.
    /usr/local/Cellar/freeglut/3.0.0/include # This another way it comes with Brew.
)

# Look for the library.
FIND_LIBRARY(FREEGLUT_LIBRARY 
  NAMES 
    freeglut
    freeglutd
    freeglut_static
    freeglut_staticd
    glut
    glutd
  HINTS
    ${CMAKE_INSTALL_PREFIX}/lib 
    $ENV{FREEGLUT_ROOT}/lib 
    /usr/local/lib
    ENV LD_LIBRARY_PATH # For OSCAR modules at Brown/CCV
    /usr/local/Cellar/freeglut/2.8.1/lib/libglut.3.dylib # Brew version.
    /usr/local/Cellar/freeglut/3.0.0/lib/libglut.3.dylib # Another Brew version.
  )

# Undo the above hack.
set(CMAKE_FIND_FRAMEWORK)

# Handle the QUIETLY and REQUIRED arguments and set FREEGLUT_FOUND to
# TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
  FREEGLUT 
  DEFAULT_MSG 
  FREEGLUT_LIBRARY 
  FREEGLUT_INCLUDE_DIR)

# Copy the results to the output variables.
IF(FREEGLUT_FOUND)
	SET(FREEGLUT_LIBRARIES ${FREEGLUT_LIBRARY})
	SET(FREEGLUT_INCLUDE_DIRS ${FREEGLUT_INCLUDE_DIR})
ELSE(FREEGLUT_FOUND)
	SET(FREEGLUT_LIBRARIES)
	SET(FREEGLUT_INCLUDE_DIRS)
ENDIF(FREEGLUT_FOUND)

## Uncomment and edit these lines for your machine if there seems to be no hope.
## set(FREEGLUT_LIBRARIES /path/to/libglut)
## set(FREEGLUT_INCLUDE_DIRS /path/to/freeglut/include/files)

MARK_AS_ADVANCED(FREEGLUT_INCLUDE_DIRS FREEGLUT_LIBRARIES)

