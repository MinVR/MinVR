# Create a library from the source files in this directory.

project (minvr_server)

set (minvr_server_source_files 
	main.cpp
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(Threads)
	set(LIBS_ALL ${CMAKE_THREAD_LIBS_INIT} dl)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_definitions(-DPLUGINPATH="${CMAKE_INSTALL_PREFIX}/plugins")

add_executable (minvr_server ${minvr_server_source_files})
target_link_libraries (minvr_server VRNet VREvent VRData VRPlugin ${LIBS_ALL})


