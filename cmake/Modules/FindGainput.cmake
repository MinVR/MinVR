# - try to find Gainput library
#
# Cache Variables:

#
# Non-cache variables you might use in your CMakeLists.txt:
# GAIN_ROOT_DIR is searched preferentially for these files
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)
#
# Original Author:
# 2009-2012 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryand.net>
# http://academic.cleardefinition.com
# Iowa State University HCI Graduate Program/VRAC
#
# Copyright Iowa State University 2009-2012.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


set(GAINPUT_ROOT_DIR
	"${GAINPOUT_ROOT_DIR}"
	CACHE
	PATH
	"Root directory to search for Gainput")

if("${CMAKE_SIZEOF_VOID_P}" MATCHES "8")
	set(_libsuffixes lib64 lib)

	# 64-bit dir: only set on win64
	file(TO_CMAKE_PATH "$ENV{ProgramW6432}" _progfiles)
else()
	set(_libsuffixes lib)
	if(NOT "$ENV{ProgramFiles\(x86\)}" STREQUAL "")
		# 32-bit dir: only set on win64
		file(TO_CMAKE_PATH "$ENV{ProgramFiles\(x86\)}" _progfiles)
	else()
		# 32-bit dir on win32, useless to us on win64
		file(TO_CMAKE_PATH "$ENV{ProgramFiles}" _progfiles)
	endif()
endif()

###
# Configure 
###

find_path(GAINPUT_INCLUDE_DIR
	NAMES
	gainput/gainput.h
	PATH_SUFFIXES
	include
	HINTS
        ${CMAKE_INSTALL_PREFIX}
	"${GAINPUT_ROOT_DIR}"
	ENV CPATH # For OSCAR modules at Brown/CCV
	PATHS
	"${_progfiles}/gainput"
	C:/usr/local
	/usr/local)

#gainput name must be out of the path
if(GAINPUT_INCLUDE_DIR)
  string(REPLACE gainput "" GAINPUT_INCLUDE_DIR ${GAINPUT_INCLUDE_DIR})
endif()

message(STATUS "GAINPUT_INCLUDE_DIR  ${GAINPUT_INCLUDE_DIR}")


find_library(GAINPUT_LIBRARY
	NAMES
	gainput
	PATH_SUFFIXES
	${_libsuffixes}
	HINTS
        ${CMAKE_INSTALL_PREFIX}
	"${GAINPUT_ROOT_DIR}"
	ENV LIBRARY_PATH # For OSCAR modules at Brown/CCV
	PATHS
	"${_progfiles}/Gainput"
	C:/usr/local
	/usr/local)




# handle the QUIETLY and REQUIRED arguments and set xxx_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Gainput
	DEFAULT_MSG
	GAINPUT_LIBRARY
	GAINPUT_INCLUDE_DIR
	${_deps_check})

if(GAINNPUT_FOUND)
	set(GAINPUT_INCLUDE_DIR "${GAINPUT_INCLUDE_DIR}" ${_deps_includes})
	set(GAINPUT_LIBRARY "${GAINPUT_LIBRARY}" ${_deps_libs})



	mark_as_advanced(GAINPUT_ROOT_DIR)
endif()

mark_as_advanced(GAINPUT_LIBRARY  GAINPUT_INCLUDE_DIR)
