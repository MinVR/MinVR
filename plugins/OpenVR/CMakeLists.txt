project(MinVR_OpenVR)

set(SOURCEFILES
  ${CMAKE_SOURCE_DIR}/third-party/GLEW/glew.c
  src/VROpenVRNodePlugin.cpp
  src/VROpenVRNode.cpp
  src/VROpenVRInputDevice.cpp
)

set (HEADERFILES
	src/VROpenVRNode.h
	src/VROpenVRInputDevice.h
)

source_group("Header Files" FILES ${HEADERFILES})

#------------------------------------------
# Download dependencies
#------------------------------------------

# OpenGL
find_package(OpenGL REQUIRED)
set(OPENGL_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
set(OPENGL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIRS})

message(STATUS "OpenGL includes (for OpenVR): " ${OPENGL_INCLUDE_DIRS})
message(STATUS "OpenGL library (for OpenVR): " ${OPENGL_LIBRARIES})

#------------------------------------------
# MinVR Dependencies
#------------------------------------------

include_directories(
	${CMAKE_SOURCE_DIR}/third-party/GLEW
)

#if (NOT MinVR_INCLUDE_DIRS)
#	message( FATAL_ERROR "MinVR_INCLUDE_DIRS must be set." )
#endif()
#if (NOT MinVR_BINARY_DIRS)
#	message( FATAL_ERROR "MinVR_BINARY_DIRS must be set." )
#endif()

#include_directories(${MinVR_INCLUDE_DIRS})
link_directories(${CMAKE_BINARY_DIR}/lib)
#link_directories(${MinVR_BINARY_DIRS})
add_definitions(-DGLEW_STATIC)

#------------------------------------------
# Include Directories
#------------------------------------------

# Include Directories
include_directories (
  src
)

SET(CMAKE_MODULE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )
find_package(OpenVR QUIET)
if(NOT OPENVR_FOUND)

  message(STATUS "OpenVR not found.  Will try building it.")
  
	set(DepName OpenVR) 

	set(${DepName}_checkout_Dir ${PROJECT_SOURCE_DIR}/lib/${DepName}/src)
	make_directory(${${DepName}_checkout_Dir})
	include(ExternalProject)
	
	set(${DepName}_build_dir ${CMAKE_BINARY_DIR}/deps/${DepName})
	set(${DepName}_stamp_dir ${${DepName}_build_dir}/stamp)
	set(${DepName}_tmp_dir ${${DepName}_build_dir}/tmp)
	
	set(${DepName}_PREFIX "${${DepName}_checkout_Dir}")
  make_directory(${CMAKE_INSTALL_PREFIX}/ext/${DepName})
	set(${DepName}_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/ext/${DepName}")
	
	set(${DepName}_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${${DepName}_INSTALL_DIR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_C_FLAGS=-fPIC -DQNANHIBIT=1)
	set(${DepName}_DIR "${${DepName}_INSTALL_DIR}")
	
	if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(${DepName}_CMAKE_ARGS ${${DepName}_CMAKE_ARGS})
	endif()
	
	set(${DepName}_PATCH "")
	   
	ExternalProject_add(${DepName}
		PREFIX ${${DepName}_PREFIX}
		GIT_REPOSITORY https://github.com/ivlab/openvr
		GIT_TAG master
		INSTALL_DIR ${${DepName}_INSTALL_DIR}
		CMAKE_ARGS ${${DepName}_CMAKE_ARGS}
		UPDATE_COMMAND ""
		DOWNLOAD_DIR ${${DepName}_checkout_Dir}
		SOURCE_DIR ${${DepName}_checkout_Dir}
		TMP_DIR ${${DepName}_tmp_dir}
		STAMP_DIR ${${DepName}_stamp_dir}
		BINARY_DIR ${${DepName}_build_dir}
	)

  message(STATUS "Building ${DepName} from: ${${DepName}_PREFIX}.")
  message(STATUS "Installing ${DepName} to: ${${DepName}_INSTALL_DIR}.")

	set(OPENVR_INCLUDE_DIRS ${${DepName}_checkout_Dir}/headers)
             
    if(MSVC)
		#set(OPENVR_LIBRARY ${${DepName}_INSTALL_DIR}/lib/openvr_api64.lib)
		set(OPENVR_LIBRARY ${${DepName}_checkout_Dir}/lib/win64/openvr_api.lib)
	else()
		set(OPENVR_LIBRARY ${${DepName}_INSTALL_DIR}/lib/libopenvr_api.a)
	endif()

	set(OPENVR_LIBRARIES ${OPENVR_LIBRARY})

endif()

#add_definitions(-D_EASYBLENDSDK_LINUX)
include_directories(${OPENVR_INCLUDE_DIRS})

message(STATUS "OpenVR includes: " ${OPENVR_INCLUDE_DIRS})
message(STATUS "OpenVR library: " ${OPENVR_LIBRARIES})


#------------------------------------------
# Specific preprocessor defines
#------------------------------------------

# Windows Section #
if (MSVC)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	find_package(X11)
	set(LIBS_ALL ${LIBS_ALL} ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xi m dl ${X11_LIBRARIES})
endif()

#------------------------------------------
# Set output directories to lib, and bin
#------------------------------------------

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/bin)
foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
	string (TOUPPER ${CONF} CONF)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/bin)
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
endforeach(CONF CMAKE_CONFIGURATION_TYPES)

#------------------------------------------
# Build Target
#------------------------------------------

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
SET(CMAKE_INSTALL_RPATH "${VRPlugins_install_dir}/${PROJECT_NAME}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library ( ${PROJECT_NAME} SHARED ${HEADERFILES} ${SOURCEFILES} )
target_link_libraries(${PROJECT_NAME} ${OPENVR_LIBRARIES} ${OPENGL_LIBRARIES} MinVR ${LIBS_ALL})

if(NOT OPENVR_FOUND)
	add_dependencies(${PROJECT_NAME} OpenVR)
endif()

#------------------------------------------
# Install Target
#------------------------------------------

install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/lib
         ARCHIVE DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/lib
         RUNTIME DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/bin)
		 
set_property(TARGET MinVR_OpenVR PROPERTY FOLDER "plugins")
