
find_path(OPENVR_INCLUDE_DIR 
    openvr.h
  HINTS 
	${CMAKE_INSTALL_PREFIX}/include 
	${CMAKE_INSTALL_PREFIX}/include/openvr
	${CMAKE_INSTALL_PREFIX}/include/OpenVR
	$ENV{OPENVR_ROOT}/include 
	$ENV{OPENVR_ROOT}/include/openvr
	$ENV{OPENVR_ROOT}/include/OpenVR
	/usr/local/include
	/usr/local/include/openvr
	/usr/local/include/OpenVR
)
set(OPENVR_LIBNAME openvr_api)
if(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(OPENVR_LIBNAME "openvr_api64" )
	endif()
endif()

find_library(OPENVR_LIBRARY 
  NAMES 
	${OPENVR_LIBNAME}
  HINTS 
	${CMAKE_INSTALL_PREFIX}/lib 
	$ENV{OPENVR_ROOT}/lib 
	/usr/local/lib
)

set(OPENVR_LIBRARIES ${OPENVR_LIBRARY})
set(OPENVR_INCLUDE_DIRS ${OPENVR_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(OPENVR  DEFAULT_MSG
                                  OPENVR_INCLUDE_DIR OPENVR_LIBRARY)

if(NOT OPENVR_FOUND)
  set(OPENVR_INCLUDE_DIR $ENV{OPENVR_INCLUDE_DIR} CACHE PATH "Set the directory location of the OpenVR include folder")
  set(OPENVR_LIB_DIR $ENV{OPENVR_LIB_DIR} CACHE PATH "Set the directory location of the OpenVR lib folder")
endif()

mark_as_advanced(OPENVR_FOUND OPENVR_INCLUDE_DIR OPENVR_LIBRARY )

