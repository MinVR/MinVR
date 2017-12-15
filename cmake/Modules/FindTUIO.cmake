
find_path(TUIO_INCLUDE_DIR 
	TuioClient.h
  HINTS 
	${CMAKE_INSTALL_PREFIX}/include 
	${CMAKE_INSTALL_PREFIX}/include/TUIO
	$ENV{TUIO_ROOT}/include 
	$ENV{TUIO_ROOT}/include/TUIO 
	/usr/local/include
	/usr/local/include/TUIO
)

find_library(TUIO_OPT_LIBRARIES 
  NAMES 
    libTUIO.a 
    TUIO.lib 
    TUIO
  HINTS 
	${CMAKE_INSTALL_PREFIX}/lib 
	$ENV{TUIO_ROOT}/lib 
	/usr/local/lib
)
          
find_library(TUIO_DEBUG_LIBRARIES 
  NAMES 
    libTUIOd.a 
    TUIOd.lib 
    TUIOd
  HINTS 
	${CMAKE_INSTALL_PREFIX}/lib 
	$ENV{TUIO_ROOT}/lib 
	/usr/local/lib
)

if(TUIO_OPT_LIBRARIES AND TUIO_DEBUG_LIBRARIES)
	set(TUIO_OPT_LIBRARIES optimized ${TUIO_OPT_LIBRARIES} )
	set(TUIO_DEBUG_LIBRARIES debug ${TUIO_DEBUG_LIBRARIES} )
#if only opt is found, use it for both
elseif(TUIO_OPT_LIBRARIES AND NOT TUIO_DEBUG_LIBRARIES)
	set(TUIO_DEBUG_LIBRARIES debug ${TUIO_OPT_LIBRARIES} )
	set(TUIO_OPT_LIBRARIES optimized ${TUIO_OPT_LIBRARIES} )
#if only debug is found, use it for both
elseif(NOT TUIO_OPT_LIBRARIES AND TUIO_DEBUG_LIBRARIES)
	set(TUIO_OPT_LIBRARIES optimized ${TUIO_DEBUG_LIBRARIES} )
	set(TUIO_DEBUG_LIBRARIES debug ${TUIO_DEBUG_LIBRARIES} )
endif()

set(TUIO_LIBRARIES ${TUIO_OPT_LIBRARIES} ${TUIO_DEBUG_LIBRARIES})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(TUIO  DEFAULT_MSG
                                  TUIO_INCLUDE_DIR TUIO_OPT_LIBRARIES TUIO_DEBUG_LIBRARIES TUIO_LIBRARIES)

mark_as_advanced(TUIO_INCLUDE_DIR TUIO_OPT_LIBRARIES TUIO_DEBUG_LIBRARIES TUIO_LIBRARIES)
