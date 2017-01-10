project (MinVR_VRPN)

#------------------------------------------
# Define the source and header files
#------------------------------------------

set (SOURCEFILES
	src/VRVRPNAnalogDevice.cpp
	src/VRVRPNButtonDevice.cpp
	src/VRVRPNPlugin.cpp
	src/VRVRPNTrackerDevice.cpp
)

set (HEADERFILES
	src/VRVRPNAnalogDevice.h
	src/VRVRPNButtonDevice.h
	src/VRVRPNTrackerDevice.h
)

source_group("Header Files" FILES ${HEADERFILES})

option(BUILD_VRPN "If enabled, VRPN will be downloaded and installed" ON)
#------------------------------------------
# Download dependencies
#------------------------------------------
if (BUILD_VRPN)
		set(vrpn_checkout_Dir ${CMAKE_BINARY_DIR}/vrpn)
		make_directory(${vrpn_checkout_Dir})
        message("   Library will be built from source in ${vrpn_checkout_Dir}.")
		include(ExternalProject)
		set(vrpn_PREFIX "${vrpn_checkout_Dir}")
		set(vrpn_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/vrpn")
		set(vrpn_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${vrpn_INSTALL_DIR} -DVRPN_BUILD_DIRECTSHOW_VIDEO_SERVER=OFF -DVRPN_BUILD_HID_GUI=OFF -DVRPN_BUILD_JAVA=OFF -DVRPN_BUILD_PYTHON=OFF -DVRPN_USE_DIRECTINPUT=OFF -DVRPN_USE_DIRECTSHOW=OFF -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -Wno-dev)
		set(vrpn_DIR "${vrpn_INSTALL_DIR}")


		if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
			set(vrpn_CMAKE_ARGS ${vrpn_CMAKE_ARGS} -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD=c++11 -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY=libc++)
		elseif(MSVC)
				set(vrpn_CMAKE_ARGS ${vrpn_CMAKE_ARGS} -DVRPN_USE_HID=ON -DVRPN_HID_DEBUGGING=ON -DVRPN_USE_LOCAL_HIDAPI=ON)
		endif()

		ExternalProject_add(vrpn
				PREFIX ${vrpn_PREFIX}
				URL https://github.com/vrpn/vrpn/releases/download/v07.33/vrpn_07_33.zip
				INSTALL_DIR ${vrpn_INSTALL_DIR}
				CMAKE_ARGS ${vrpn_CMAKE_ARGS}
				UPDATE_COMMAND ""
		 )		 
		set_property(TARGET vrpn PROPERTY FOLDER "Dependencies")
		set(VRPN_INCLUDE_DIRS ${vrpn_INSTALL_DIR}/include CACHE INTERNAL "Directory of VRPN header files")
		if(MSVC)
			set(VRPN_LIBRARIES optimized ${vrpn_INSTALL_DIR}/lib/vrpn.lib optimized ${vrpn_INSTALL_DIR}/lib/quat.lib debug ${vrpn_INSTALL_DIR}/lib/vrpnd.lib debug  ${vrpn_INSTALL_DIR}/lib/quatd.lib CACHE INTERNAL "VRPN lib file")
			set(All_LIB ${MinVR_LIBRARIES})
		else()
#			set(VRPN_LIBRARY optimized ${vrpn_INSTALL_DIR}/lib/libvrpn.a debug ${vrpn_INSTALL_DIR}/lib/libvrpnd.a CACHE INTERNAL "VRPN lib file")
			set(VRPN_LIBRARIES ${vrpn_INSTALL_DIR}/lib/libvrpn.a ${vrpn_INSTALL_DIR}/lib/libquat.a CACHE INTERNAL "VRPN lib file")
		endif()	
		add_definitions(-DUSE_VRPN)
		#set(LIBS_ALL ${LIBS_ALL} ${VRPN_LIBRARY})
		include_directories(${VRPN_INCLUDE_DIRS})
else()
	SET(CMAKE_MODULE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )
	find_package(VRPN REQUIRED)
	add_definitions(-DUSE_VRPN)
	include_directories(${VRPN_INCLUDE_DIRS})
endif()

#------------------------------------------
# MinVR Dependencies
#------------------------------------------

link_directories(${CMAKE_BINARY_DIR}/lib)

#------------------------------------------
# Include Directories
#------------------------------------------

# Include Directories
include_directories (
  ${vr_src_dir}
  src
  deps/vrpn
)

#------------------------------------------
# Specific preprocessor defines
#------------------------------------------

# Windows Section #
if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${WINDOWS_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	find_library(COCOA_LIB Cocoa)
	find_library(IOKIT_LIB IOKit)
	find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
    	find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
	set(LIBS_ALL ${LIBS_ALL} ${COCOA_LIB} ${IOKIT_LIB} ${CORE_FOUNDATION_FRAMEWORK} ${CORE_VIDEO_FRAMEWORK})
	message(STATUS "${CORE_VIDEO_FRAMEWORK}")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(Threads)
	find_package(X11)
	set(LIBS_ALL ${LIBS_ALL} ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xi m dl ${X11_LIBRARIES})
endif()

#------------------------------------------
# Set output directories to lib, and bin
#------------------------------------------

make_directory(${CMAKE_BINARY_DIR}/lib)
make_directory(${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/bin)
foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
	string (TOUPPER ${CONF} CONF)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/bin)
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
endforeach(CONF CMAKE_CONFIGURATION_TYPES)

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")
set(CMAKE_MINSIZEREL_POSTFIX "s")

#set the build postfix extension according to the current configuration
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

#------------------------------------------
# Build Target
#------------------------------------------

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library ( ${PROJECT_NAME} SHARED ${HEADERFILES} ${SOURCEFILES} )
target_link_libraries(${PROJECT_NAME} ${VRPN_LIBRARIES} MinVR ${LIBS_ALL})
if (BUILD_VRPN)
	add_dependencies( ${PROJECT_NAME} vrpn )
endif()
#------------------------------------------
# Install Target
#------------------------------------------

install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/lib
         ARCHIVE DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/lib
         RUNTIME DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/bin)

set_property(TARGET MinVR_VRPN PROPERTY FOLDER "plugins")
if (BUILD_VRPN)
	set_property(TARGET vrpn PROPERTY FOLDER "plugins/dependencies/VRPN")
endif()
