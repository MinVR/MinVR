# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Builds the MinVR_OpenGL plugin as a shared library.

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)




# Plugin names must begin with "MinVR_"
project(MinVR_OpenGL)


# Source:
set (SOURCEFILES 
  src/VROpenGLGraphicsToolkit.cpp
  src/VROpenGLPlugin.cpp
)
set (HEADERFILES
  src/VROpenGLGraphicsToolkit.h
  src/VROpenGLHeaders.h
)


# Define the target
add_library(${PROJECT_NAME} SHARED ${HEADERFILES} ${SOURCEFILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>  # for headers when building
    $<INSTALL_INTERFACE:include/MinVR${MINVR_VERSION_STR}/${PROJECT_NAME}>  # for client in install mode
)


# Setup dependency on libMinVR:
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>  # for headers when building
    $<INSTALL_INTERFACE:include/MinVR${MINVR_VERSION_STR}>  # for client in install mode
)
target_link_libraries(${PROJECT_NAME} MinVR)


# Setup dependency on OpenGL:
include(UseOpenGL)
UseOpenGL(${PROJECT_NAME} PUBLIC)


# Setup dependency on GLEW (only needed on Windows):
if (WIN32)
  include(UseGLEW)
  UseGLEW(${PROJECT_NAME} PUBLIC)
endif()


# Installation:
install(TARGETS ${PROJECT_NAME} EXPORT MinVRTargets COMPONENT ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/MinVR${MINVR_VERSION_STR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/MinVR${MINVR_VERSION_STR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
install(FILES ${HEADERFILES} DESTINATION include/MinVR${MINVR_VERSION_STR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})


# For better organization when using an IDE with folder structures:
source_group("Header Files" FILES ${HEADERFILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "plugins")

