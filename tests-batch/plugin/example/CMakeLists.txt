# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

project (test_plugin)

set (test_pluginlib_source_files 
	TestPlugin.cpp
)

set (test_pluginapp_source_files 
	main.cpp
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(LIBS_ALL dl)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_definitions(-DPLUGINPATH="${CMAKE_BINARY_DIR}")

add_executable (test_pluginapp ${test_pluginapp_source_files})
target_link_libraries (test_pluginapp VRPlugin ${LIBS_ALL})

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library ( test_plugin SHARED ${HEADERFILES} ${test_pluginlib_source_files} )
target_link_libraries (test_plugin VRPlugin)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-batch”)

