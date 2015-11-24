# Create a library from the source files in this directory.

project (VRPlus)


#-----------------Extensions--------------------------
set(MinVRExtensions_checkout_Dir ${CMAKE_BINARY_DIR}/ext)
make_directory(${MinVRExtensions_checkout_Dir})
include(ExternalProject)

set(MinVRExtensions_PREFIX "${MinVRExtensions_checkout_Dir}")
set(MinVRExtensions_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/plugins")

set(MinVRExtensions_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${MinVRExtensions_INSTALL_DIR} -DMinVR_INCLUDE_DIRS=${PROJECT_SOURCE_DIR}/.. -DMinVR_BINARY_DIRS=${CMAKE_BINARY_DIR}/lib -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
set(MinVRExtensions_DIR "${MinVRExtensions_INSTALL_DIR}")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(MinVRExtensions_CMAKE_ARGS ${MinVRExtensions_CMAKE_ARGS} -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD="c++11" -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY="libc++")
endif()
   
ExternalProject_add(MinVRExtensions
		PREFIX ${MinVRExtensions_PREFIX}
		GIT_REPOSITORY https://github.com/dtorban/MinVRExtensions.git
		GIT_TAG minvr2
		INSTALL_DIR ${MinVRExtensions_INSTALL_DIR}
		CMAKE_ARGS ${MinVRExtensions_CMAKE_ARGS}
		UPDATE_COMMAND ""
)
 
set_property(TARGET MinVRExtensions PROPERTY FOLDER "Plugins")

add_dependencies(MinVRExtensions VREvent)