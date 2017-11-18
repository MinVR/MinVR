# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Builds the MinVR_OpenVR plugin as a shared library.

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)




# By convention, plugin names begin with "MinVR_"
project(MinVR_OpenVR)


# Source:
set(SOURCEFILES
  src/VROpenVRNodePlugin.cpp
  src/VROpenVRNode.cpp
  src/VROpenVRInputDevice.cpp
  src/VROpenVRRenderModel.cpp
  src/VROpenVRRenderModelHandler.cpp
)
set (HEADERFILES
	src/VROpenVRNode.h
	src/VROpenVRInputDevice.h
	src/VROpenVRRenderModel.h
	src/VROpenVRRenderModelHandler.h
)


# Setup dependency on libMinVR:
list(APPEND MY_INC_DIRS src)
list(APPEND MY_LIB_DIRS ${CMAKE_BINARY_DIR})
list(APPEND MY_LIBS MinVR)


# Setup dependency on OpenVR:
include(UseOpenVR)
UseOpenVR()
list(APPEND MY_INC_DIRS ${OPENVR_INCLUDE_DIR})
list(APPEND MY_LIB_DIRS ${OPENVR_LIBRARY_DIR})
list(APPEND MY_LIBS ${OPENVR_LIBRARIES})

# Setup dependency on OpenGL:
include(UseOpenGL)
UseOpenGL()
list(APPEND MY_INC_DIRS ${OPENGL_INCLUDE_DIR})
list(APPEND MY_LIB_DIRS ${OPENGL_LIBRARY_DIR})
list(APPEND MY_LIBS ${OPENGL_LIBRARIES})

# Setup dependency on GLEW (only needed on Windows):
if (WIN32)
  include(UseGLEW)
  UseGLEW()
  list(APPEND MY_INC_DIRS ${GLEW_INCLUDE_DIR})
  list(APPEND MY_LIB_DIRS ${GLEW_LIBRARY_DIR})
  list(APPEND MY_LIBS ${GLEW_LIBRARIES})
endif()

# Setup dependency on C++-11:
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif()
endif()


# Create the shared library:
include_directories(${MY_INC_DIRS})
link_directories(${MY_LIB_DIRS})
add_library(${PROJECT_NAME} SHARED ${HEADERFILES} ${SOURCEFILES})
target_link_libraries(${PROJECT_NAME} ${MY_LIBS} ${LIBS_ALL})


# If we have dependencies that were not found pre-installed on the system
# and that will be built as part of the MinVR build process, then these
# must be listed as dependencies of the current project so that cmake
# builds them first.
if (${OPENVR_AUTOBUILT})
    add_dependencies(${PROJECT_NAME} OpenVR)
endif()


# Installation:
install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


# For better organization when using an IDE with folder structures:
source_group("Header Files" FILES ${HEADERFILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "plugins")


