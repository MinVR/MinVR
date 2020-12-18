find_path(HOLOPLAY_DIR 
	NAMES
		HoloPlayCore/include/HoloPlayCore.h
)

find_path(HOLOPLAY_INCLUDE_DIR 
	NAMES
		HoloPlayCore.h
	HINTS
		${HOLOPLAY_DIR}/HoloPlayCore/include
)

if(APPLE)
	find_library(HOLOPLAY_LIBRARY
	NAMES
		libHoloPlayCore.so
	HINTS
		${HOLOPLAY_DIR}/HoloPlayCore/dylib/linux/
	)
elseif(UNIX)
	find_library(HOLOPLAY_LIBRARY
	NAMES
		libHoloPlayCore.dylib
	HINTS
		${HOLOPLAY_DIR}/HoloPlayCore/dylib/macos/
	)
elseif(MSVC)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		find_library(HOLOPLAY_LIBRARY
		NAMES
			HoloPlayCore.lib
		HINTS
			${HOLOPLAY_DIR}/HoloPlayCore/dylib/Win64/
		)
	else()
		find_library(HOLOPLAY_LIBRARY
		NAMES
			HoloPlayCore.lib
		HINTS
			${HOLOPLAY_DIR}/HoloPlayCore/dylib/Win32/
		)
	endif()
endif()

message(STATUS ${HOLOPLAY_LIBRARY})
message(STATUS ${HOLOPLAY_INCLUDE_DIR})

find_package_handle_standard_args(HOLOPLAY_DEFAULT_MSG
          		  HOLOPLAY_LIBRARY HOLOPLAY_INCLUDE_DIR)
				  
if(NOT HOLOPLAY_FOUND)
  set(HOLOPLAY_DIR $ENV{HOLOPLAY_DIR} CACHE PATH "Set the directory location of the HoloPlayCoreSDK")
endif()

mark_as_advanced(HOLOPLAY_LIBRARY HOLOPLAY_INCLUDE_DIR)

