# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Builds the MinVR_Scalable plugin as a shared library.
# This plugin requires a proprietary library from Scalable Display Technologies of 
# Cambridge, Massachusetts (scalabledisplay.com).

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)



# By convention, plugin names begin with "MinVR_"
project(MinVR_Scalable)


# Source:
set (SOURCEFILES 
  src/VRScalablePlugin.cpp
  src/VRScalableNode.cpp
)

set (HEADERFILES
  src/VRScalableHeaders.h
  src/VRScalableNode.h
)


# Setup dependency on libMinVR:
list(APPEND MY_INC_DIRS src)
list(APPEND MY_LIB_DIRS ${CMAKE_BINARY_DIR})
list(APPEND MY_LIBS MinVR)

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

# Setup dependency on Scalable:
# This follows a different pattern because there is no option to auto-download-and-build
# the libScalable.
find_package(LibSCALABLE REQUIRED)
add_definitions(-D_EASYBLENDSDK_LINUX)
list(APPEND MY_INC_DIRS ${LIBSCALABLE_INCLUDE_DIRS})
list(APPEND MY_LIBS ${LIBSCALABLE_ALL_LIBRARIES})


# Create the shared library:
include_directories(${MY_INC_DIRS})
link_directories(${MY_LIB_DIRS})
add_library(${PROJECT_NAME} SHARED ${HEADERFILES} ${SOURCEFILES})
target_link_libraries(${PROJECT_NAME} ${MY_LIBS} ${LIBS_ALL})



# Installation:
install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


# For better organization when using an IDE with folder structures:
source_group("Header Files" FILES ${HEADERFILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "plugins")

