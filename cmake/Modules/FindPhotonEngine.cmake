
find_path(PHOTON_DIR 
	NAMES
		LoadBalancing-cpp/src/Client.h
)

set(PHOTON_LIBRARY_NAME_RELEASE libPhotonRelease32.a)
set(PHOTON_LIBRARY_NAME_DEBUG libPhotonRelease32.a)
set(PHOTON_LOADBALANCING_LIBRARY_NAME_RELEASE libLoadBalancingRelease32.a)
set(PHOTON_LOADBALANCING_LIBRARY_NAME_DEBUG libLoadBalancingDebug32.a)
set(PHOTON_COMMON_LIBRARY_NAME_RELEASE libCommonRelease32.a)
set(PHOTON_COMMON_LIBRARY_NAME_DEBUG libCommonDebug32.a)

if(APPLE)
	set(PHOTON_LIBRARY_NAME_RELEASE libPhoton-cpp_release_macosx.a)
	set(PHOTON_LIBRARY_NAME_DEBUG libPhoton-cpp_debug_macosx.a)
	set(PHOTON_LOADBALANCING_LIBRARY_NAME_RELEASE libLoadBalancing-cpp_release_macosx.a)
	set(PHOTON_LOADBALANCING_LIBRARY_NAME_DEBUG libLoadBalancing-cpp_debug_macosx.a)
	set(PHOTON_COMMON_LIBRARY_NAME_RELEASE libCommon-cpp_release_macosx.a)
	set(PHOTON_COMMON_LIBRARY_NAME_DEBUG libCommon-cpp_debug_macosx.a)
elseif(UNIX)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(PHOTON_LIBRARY_NAME_RELEASE libPhotonRelease64.a)
		set(PHOTON_LIBRARY_NAME_DEBUG libPhotonRelease64.a)
		set(PHOTON_LOADBALANCING_LIBRARY_NAME_RELEASE libLoadBalancingRelease64.a)
		set(PHOTON_LOADBALANCING_LIBRARY_NAME_DEBUG libLoadBalancingDebug64.a)
		set(PHOTON_COMMON_LIBRARY_NAME_RELEASE libCommonRelease64.a)
		set(PHOTON_COMMON_LIBRARY_NAME_DEBUG libCommonDebug64.a)
	endif()
elseif(MSVC)
	set(MSVC_VERSION_STRING vc16)
	if (MSVC_VERSION GREATER_EQUAL 1920) #vs2019 vc16 
		set(MSVC_VERSION_STRING vc16)
	elseif (MSVC_VERSION GREATER_EQUAL 1910)#vs2017 vc15
		set(MSVC_VERSION_STRING vc15)
	elseif (MSVC_VERSION GREATER_EQUAL 1900)#vs2015 vc14
		set(MSVC_VERSION_STRING vc14)
	elseif (MSVC_VERSION GREATER_EQUAL 1800)#vs2013 vc12
		set(MSVC_VERSION_STRING vc13)
	endif()
	
	set(ARC_STRING Win32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(ARC_STRING x64)
	endif()
	
	set(PHOTON_LIBRARY_NAME_RELEASE Photon-cpp_${MSVC_VERSION_STRING}_release_windows_md_${ARC_STRING}.lib)
	set(PHOTON_LIBRARY_NAME_DEBUG Photon-cpp_${MSVC_VERSION_STRING}_debug_windows_md_${ARC_STRING}.lib)
	set(PHOTON_LOADBALANCING_LIBRARY_NAME_RELEASE LoadBalancing-cpp_${MSVC_VERSION_STRING}_release_windows_md_${ARC_STRING}.lib)
	set(PHOTON_LOADBALANCING_LIBRARY_NAME_DEBUG LoadBalancing-cpp_${MSVC_VERSION_STRING}_debug_windows_md_${ARC_STRING}.lib)
	set(PHOTON_COMMON_LIBRARY_NAME_RELEASE Common-cpp_${MSVC_VERSION_STRING}_release_windows_md_${ARC_STRING}.lib)
	set(PHOTON_COMMON_LIBRARY_NAME_DEBUG Common-cpp_${MSVC_VERSION_STRING}_debug_windows_md_${ARC_STRING}.lib)
endif()

message(STATUS ${PHOTON_LIBRARY_NAME_DEBUG})
message(STATUS ${PHOTON_LOADBALANCING_LIBRARY_NAME_DEBUG})
message(STATUS ${PHOTON_COMMON_LIBRARY_NAME_DEBUG})

find_library(PHOTON_LIBRARY_RELEASE
	NAMES
		${PHOTON_LIBRARY_NAME_RELEASE}
	HINTS
		${PHOTON_DIR}/Photon-cpp/lib/
		${PHOTON_DIR}/Photon-cpp/
)

find_library(PHOTON_LOADBALANCING_LIBRARY_RELEASE
	NAMES
		${PHOTON_LOADBALANCING_LIBRARY_NAME_RELEASE}
	HINTS
		${PHOTON_DIR}/LoadBalancing-cpp/lib/
		${PHOTON_DIR}/LoadBalancing-cpp/
	 
)

find_library(PHOTON_COMMON_LIBRARY_RELEASE
	NAMES
		${PHOTON_COMMON_LIBRARY_NAME_RELEASE}
	HINTS
		${PHOTON_DIR}/Common-cpp/lib/
		${PHOTON_DIR}/Common-cpp/
)

find_library(PHOTON_LIBRARY_DEBUG
	NAMES
		${PHOTON_LIBRARY_NAME_DEBUG}
	HINTS
		${PHOTON_DIR}/Photon-cpp/lib/
		${PHOTON_DIR}/Photon-cpp/
)

find_library(PHOTON_LOADBALANCING_LIBRARY_DEBUG
	NAMES
		${PHOTON_LOADBALANCING_LIBRARY_NAME_DEBUG}
	HINTS
		${PHOTON_DIR}/LoadBalancing-cpp/lib/
		${PHOTON_DIR}/LoadBalancing-cpp/
)

find_library(PHOTON_COMMON_LIBRARY_DEBUG
	NAMES
		${PHOTON_COMMON_LIBRARY_NAME_DEBUG}
	HINTS
		${PHOTON_DIR}/Common-cpp/lib/
		${PHOTON_DIR}/Common-cpp/
)

set(PHOTON_LIBRARIES optimized  ${PHOTON_LIBRARY_RELEASE} optimized ${PHOTON_LOADBALANCING_LIBRARY_RELEASE} optimized ${PHOTON_COMMON_LIBRARY_RELEASE}
												debug  ${PHOTON_LIBRARY_DEBUG} debug ${PHOTON_LOADBALANCING_LIBRARY_DEBUG} debug ${PHOTON_COMMON_LIBRARY_DEBUG})
set(PHOTON_INCLUDE_DIRS ${PHOTON_DIR})

find_package_handle_standard_args(PHOTON DEFAULT_MSG
                                  PHOTON_LIBRARIES PHOTON_INCLUDE_DIRS PHOTON_LIBRARY_RELEASE PHOTON_LOADBALANCING_LIBRARY_RELEASE PHOTON_COMMON_LIBRARY_RELEASE
									PHOTON_LIBRARY_DEBUG PHOTON_LOADBALANCING_LIBRARY_DEBUG PHOTON_COMMON_LIBRARY_DEBUG)

if(NOT PHOTON_FOUND)
  set(PHOTON_DIR $ENV{PHOTON_DIR} CACHE PATH "Set the directory location of the PhotonEngine")
endif()

mark_as_advanced(PHOTON_INCLUDE_DIRS PHOTON_LIBRARY_RELEASE PHOTON_LOADBALANCING_LIBRARY_RELEASE PHOTON_COMMON_LIBRARY_RELEASE
					PHOTON_LIBRARY_DEBUG PHOTON_LOADBALANCING_LIBRARY_DEBUG PHOTON_COMMON_LIBRARY_DEBUG)

