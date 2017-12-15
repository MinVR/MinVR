# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

project (plugin_info)

set (plugin_info_source_files 
	main.cpp
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(Threads)
	set(LIBS_ALL ${CMAKE_THREAD_LIBS_INIT} dl)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_definitions(-DPLUGINPATH="${CMAKE_INSTALL_PREFIX}/plugins")

add_executable (plugin_info ${plugin_info_source_files})
target_link_libraries (plugin_info VREvent VRData VRPlugin ${LIBS_ALL})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-batch”)
