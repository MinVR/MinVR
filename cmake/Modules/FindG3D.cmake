

# Includes
find_path(G3D_INCLUDE_DIR 
    G3D/G3D.h
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/include/G3D-8.0
    ${CMAKE_INSTALL_PREFIX}/include/G3D
    ${CMAKE_INSTALL_PREFIX}/include 
    $ENV{G3D_ROOT}/include/G3D-8.0
    $ENV{G3D_ROOT}/include/G3D
    $ENV{G3D_ROOT}/include
    /usr/local/include/G3D-8.0
    /usr/local/include/G3D
    /usr/local/include
)


# libGLG3D
find_library(GLG3D_OPT_LIBRARIES 
  NAMES 
    GLG3D
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
    ${CMAKE_INSTALL_PREFIX}/lib/G3D/Release
    ${CMAKE_INSTALL_PREFIX}/lib/G3D
    ${CMAKE_INSTALL_PREFIX}/lib/Release
    ${CMAKE_INSTALL_PREFIX}/lib
    $ENV{G3D_ROOT}/lib/Release
    $ENV{G3D_ROOT}/lib
    /usr/local/lib
)

find_library(GLG3D_DEBUG_LIBRARIES 
  NAMES 
    GLG3Dd
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Debug
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
    ${CMAKE_INSTALL_PREFIX}/lib/G3D/Debug
    ${CMAKE_INSTALL_PREFIX}/lib/G3D
    ${CMAKE_INSTALL_PREFIX}/lib/Debug
    ${CMAKE_INSTALL_PREFIX}/lib
    $ENV{G3D_ROOT}/lib/Debug
    $ENV{G3D_ROOT}/lib
    /usr/local/lib
)   

# Includes
find_path(GLG3D_INCLUDE_DIR 
    GLG3D/GLG3D.h
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/include/G3D-8.0
    ${CMAKE_INSTALL_PREFIX}/include/G3D
    ${CMAKE_INSTALL_PREFIX}/include 
    $ENV{G3D_ROOT}/include/G3D-8.0
    $ENV{G3D_ROOT}/include/G3D
    $ENV{G3D_ROOT}/include
    /usr/local/include/G3D-8.0
    /usr/local/include/G3D
    /usr/local/include
)


# libG3D
find_library(G3D_OPT_LIBRARIES 
  NAMES 
    G3D
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
    ${CMAKE_INSTALL_PREFIX}/lib/G3D/Release
    ${CMAKE_INSTALL_PREFIX}/lib/G3D
    ${CMAKE_INSTALL_PREFIX}/lib/Release
    ${CMAKE_INSTALL_PREFIX}/lib
    $ENV{G3D_ROOT}/lib/Release
    $ENV{G3D_ROOT}/lib
    /usr/local/lib
)

find_library(G3D_DEBUG_LIBRARIES 
  NAMES 
    G3Dd
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Debug
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
    ${CMAKE_INSTALL_PREFIX}/lib/G3D/Debug
    ${CMAKE_INSTALL_PREFIX}/lib/G3D
    ${CMAKE_INSTALL_PREFIX}/lib/Debug
    ${CMAKE_INSTALL_PREFIX}/lib
    $ENV{G3D_ROOT}/lib/Debug
    $ENV{G3D_ROOT}/lib
    /usr/local/lib
)       


# Dependencies
find_library(ZLIB_LIBRARIES 
  NAMES 
    z 
	zlib
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
	${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
    ${CMAKE_INSTALL_PREFIX}/lib
    $ENV{G3D_ROOT}/lib/Release
	$ENV{G3D_ROOT}/lib/Release
    /usr/local/lib
)  

find_library(ZIP_LIBRARIES 
  NAMES 
    zip
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
	${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
    ${CMAKE_INSTALL_PREFIX}/lib
    $ENV{G3D_ROOT}/lib/Release
	$ENV{G3D_ROOT}/lib/Release
    /usr/local/lib
)  

find_library(PNG_LIBRARIES 
  NAMES 
    png
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
	${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
    ${CMAKE_INSTALL_PREFIX}/lib
    $ENV{G3D_ROOT}/lib/Release
	$ENV{G3D_ROOT}/lib/Release
    /usr/local/lib
)  

find_library(JPEG_LIBRARIES 
  NAMES 
    jpeg
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
	${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
    ${CMAKE_INSTALL_PREFIX}/lib
    $ENV{G3D_ROOT}/lib/Release
	$ENV{G3D_ROOT}/lib/Release
    /usr/local/lib
) 

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    find_library(AVFORMAT_LIBRARIES 
      NAMES 
        avformat
      HINTS 
		${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
		${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
		${CMAKE_INSTALL_PREFIX}/lib
		$ENV{G3D_ROOT}/lib/Release
		$ENV{G3D_ROOT}/lib/Release
		/usr/local/lib
    ) 

    find_library(AVCODEC_LIBRARIES 
      NAMES 
        avcodec
      HINTS 
		${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
		${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
		${CMAKE_INSTALL_PREFIX}/lib
		$ENV{G3D_ROOT}/lib/Release
		$ENV{G3D_ROOT}/lib/Release
		/usr/local/lib    
	) 
    find_library(AVUTIL_LIBRARIES 
      NAMES 
        avutil
      HINTS 
		${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
		${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
		${CMAKE_INSTALL_PREFIX}/lib
		$ENV{G3D_ROOT}/lib/Release
		$ENV{G3D_ROOT}/lib/Release
		/usr/local/lib
    ) 
else()
  	if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        find_library(MINGWRT_LIBRARIES 
          NAMES 
            mingwrt
          HINTS 
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
			${CMAKE_INSTALL_PREFIX}/lib
			$ENV{G3D_ROOT}/lib/Release
			$ENV{G3D_ROOT}/lib/Release
			/usr/local/lib
        )

        find_library(AVFORMAT_LIBRARIES 
          NAMES 
            avformat
          HINTS 
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
			${CMAKE_INSTALL_PREFIX}/lib
			$ENV{G3D_ROOT}/lib/Release
			$ENV{G3D_ROOT}/lib/Release
			/usr/local/lib
        ) 

        find_library(AVCODEC_LIBRARIES 
          NAMES 
            avcodec
          HINTS 
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
			${CMAKE_INSTALL_PREFIX}/lib
			$ENV{G3D_ROOT}/lib/Release
			$ENV{G3D_ROOT}/lib/Release
			/usr/local/lib
        ) 

        find_library(AVUTIL_LIBRARIES 
          NAMES 
            avutil
          HINTS 
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
			${CMAKE_INSTALL_PREFIX}/lib
			$ENV{G3D_ROOT}/lib/Release
			$ENV{G3D_ROOT}/lib/Release
			/usr/local/lib
        ) 
    elseif (COMPILE_WITH_64FFMPEG)
        find_library(AVFORMAT_LIBRARIES 
          NAMES 
            avformat
          HINTS 
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
			${CMAKE_INSTALL_PREFIX}/lib
			$ENV{G3D_ROOT}/lib/Release
			$ENV{G3D_ROOT}/lib/Release
			/usr/local/lib
        ) 

        find_library(AVCODEC_LIBRARIES 
          NAMES 
            avcodec
          HINTS 
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
			${CMAKE_INSTALL_PREFIX}/lib
			$ENV{G3D_ROOT}/lib/Release
			$ENV{G3D_ROOT}/lib/Release
			/usr/local/lib
        ) 

        find_library(AVUTIL_LIBRARIES 
          NAMES 
            avutil
          HINTS 
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
			${CMAKE_INSTALL_PREFIX}/lib
			$ENV{G3D_ROOT}/lib/Release
			$ENV{G3D_ROOT}/lib/Release
			/usr/local/lib
        ) 

        find_library(SWSCALE_LIBRARIES 
          NAMES 
            swscale
          HINTS 
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0/Release
			${CMAKE_INSTALL_PREFIX}/lib/G3D-8.0
			${CMAKE_INSTALL_PREFIX}/lib
			$ENV{G3D_ROOT}/lib/Release
			$ENV{G3D_ROOT}/lib/Release
			/usr/local/lib
        )
  	endif()
endif()


set(G3D_INCLUDE_DIRS
    ${G3D_INCLUDE_DIR}
    /opt/X11/include
	${GLG3D_INCLUDE_DIR}
)




set(G3D_LIBRARIES optimized ${GLG3D_OPT_LIBRARIES} optimized ${G3D_OPT_LIBRARIES})

if (GLG3D_DEBUG_LIBRARIES)
  list(APPEND G3D_LIBRARIES debug ${GLG3D_DEBUG_LIBRARIES})
endif()

if (G3D_DEBUG_LIBRARIES)
  list(APPEND G3D_LIBRARIES debug ${G3D_DEBUG_LIBRARIES})
endif()

set(G3D_EXTRA_LIBRARIES ${LIB_ZLIB} ${LIB_ZIP} ${LIB_PNG} ${LIB_JPEG})

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(G3D_EXTRA_LIBRARIES ${G3D_EXTRA_LIBRARIES} ${LIB_AVFORMAT} ${LIB_AVCODEC} ${LIB_AVUTIL})
else() 
	if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(G3D_EXTRA_LIBRARIES ${G3D_EXTRA_LIBRARIES} ${LIB_AVFORMAT} ${LIB_AVCODEC} ${LIB_AVUTIL} ${LIB_MINGWRT})
	elseif (COMPILE_WITH_64FFMPEG)
		set(G3D_EXTRA_LIBRARIES ${G3D_EXTRA_LIBRARIES} ${LIB_AVFORMAT} ${LIB_AVCODEC} ${LIB_AVUTIL} ${LIB_SWSCALE} ${LIB_MINGWRT})
	endif()
endif()



if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  if (${DARWIN_MAJOR_VERSION} MATCHES "12")
    # Mountain Lion doesn't include X11 anymore, so we'll assume the user is
    # using MacPorts and has X11 installed.
    include_directories("/opt/local/include/")
  endif (${DARWIN_MAJOR_VERSION} MATCHES "12")
	find_library(CARBON_LIBRARIES Carbon)
	find_library(COCOA_LIBRARIES Cocoa)
	set(G3D_EXTRA_LIBRARIES ${G3D_EXTRA_LIBRARIES} ${CARBON_LIBRARIES} ${COCOA_LIBRARIES})
endif()

list(APPEND G3D_LIBRARIES ${G3D_EXTRA_LIBRARIES})



include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBNAME_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
    G3D  
    DEFAULT_MSG
    G3D_INCLUDE_DIRS
    G3D_LIBRARIES
	GLG3D_INCLUDE_DIR
)

mark_as_advanced(
    G3D
    G3D_INCLUDE_DIR
    G3D_INCLUDE_DIRS
    G3D_OPT_LIBRARIES 
    G3D_DEBUG_LIBRARIES
	GLG3D_INCLUDE_DIR
    GLG3D_OPT_LIBRARIES
    GLG3D_DEBUG_LIBRARIES
    G3D_EXTRA_LIBRARIES 
    G3D_LIBRARIES
    ZLIB_LIBRARIES
    ZIP_LIBRARIES
    PNG_LIBRARIES
    JPEG_LIBRARIES
    AVFORMAT_LIBRARIES
    AVCODEC_LIBRARIES
    AVUTIL_LIBRARIES
    MINGWRT_LIBRARIES
    SWSCALE_LIBRARIES
    CARBON_LIBRARIES
    COCOA_LIBRARIES
)
