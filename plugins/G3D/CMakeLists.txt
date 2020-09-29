# Template for creating a CMakeLists.txt for new plugins -- copy this, replace XXX and YYY, 
# and follow the example of how OpenGL/GLEW are handled below if you need your plugin to 
# link with an external library.
# -----------------------------------------------------------------------------------------

# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Builds the MinVR_G3D plugin as a shared library.

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)


# Plugin names must begin with "MinVR_"
project(MinVR_G3D)


# Source:
set (VRG3DSOURCEFILES 
  VRG3D/src/VRG3DPlugin.cpp
  VRG3D/src/VRG3DWindowToolkit.cpp
  VRG3D/src/VRG3DInputDevice.cpp
  VRG3D/src/VRG3DGraphicsToolkit.cpp
  VRG3D/src/VRG3DDisplayNode.cpp
  VRG3D/src/VRG3DApp.cpp
  VRG3D/src/ConfigMap.cpp
  VRG3D/src/DisplayTile.cpp
  VRG3D/src/G3DOperators.cpp
  VRG3D/src/ProjectionVRCamera.cpp
  VRG3D/src/VRG3DEvent.cpp
  VRG3D/src/InputDevice.cpp
  VRG3D/src/SynchedSystem.cpp
  VRG3D/src/MouseToTracker.cpp
  
)
set (VRG3DHEADERFILES 
  VRG3D/include/VRG3DWindowToolkit.h
  VRG3D/include/VRG3DInputDevice.h
  VRG3D/include/VRG3DGraphicsToolkit.h
  VRG3D/include/VRG3DDisplayNode.h
  VRG3D/include/VRG3DApp.h
  VRG3D/include/ConfigMap.h
  VRG3D/include/DisplayTile.h
  VRG3D/include/G3DOperators.h
  VRG3D/include/ProjectionVRCamera.h
  VRG3D/include/VRG3DEvent.h
  VRG3D/include/InputDevice.h
  VRG3D/include/SynchedSystem.h
  VRG3D/include/CommonInc.H
  VRG3D/include/MouseToTracker.h
)



# Define the target
add_library(${PROJECT_NAME} SHARED ${VRG3DHEADERFILES} ${VRG3DSOURCEFILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>                  # for headers when building
    $<INSTALL_INTERFACE:${INSTALL_PLUGININCLUDE_DEST}/${PROJECT_NAME}>  # for client in install mode
)

set(VRG3D_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(VRG3D_INCLUDE "/VRG3D/include")

message (STATUS "1.VRG3D_INCLUDE_DIR " ${VRG3D_INCLUDE_DIR}${VRG3D_INCLUDE})
set_property(GLOBAL PROPERTY VRG3D_INCLUDE_DIR_PROPERTY "${VRG3D_INCLUDE_DIR}${VRG3D_INCLUDE}")

message(STATUS "Setting up filters")
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DG3D_NO_FFMPEG)

# Setup dependency on libMinVR:
target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>  # for headers when building
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DEST}>              # for client in install mode
)

target_link_libraries(${PROJECT_NAME} PUBLIC MinVR)


# Setup dependency on G3D:
include(AutoBuildG3D)
AutoBuild_use_package_G3D(${PROJECT_NAME} PUBLIC)



# G3D depencies


# Installation:
install(TARGETS ${PROJECT_NAME} EXPORT MinVRTargets COMPONENT ${PROJECT_NAME}
    LIBRARY DESTINATION ${INSTALL_PLUGINLIB_DEST}
    ARCHIVE DESTINATION ${INSTALL_PLUGINLIB_DEST}
    RUNTIME DESTINATION ${INSTALL_PLUGINDLL_DEST}
)
install(FILES ${VRG3DHEADERFILES} DESTINATION ${INSTALL_PLUGININCLUDE_DEST}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})


# For better organization when using an IDE with folder structures:
source_group("Header Files" FILES ${VRG3DHEADERFILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "plugins")
