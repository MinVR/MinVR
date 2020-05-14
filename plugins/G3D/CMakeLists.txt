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
  VRG3D/src/Event.cpp
  VRG3D/src/InputDevice.cpp
  VRG3D/src/SynchedSystem.cpp

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
  VRG3D/include/Event.h
  VRG3D/include/InputDevice.h
  VRG3D/include/SynchedSystem.h
  VRG3D/include/CommonInc.H
)

set (VRG3DBASESOURCEFILES 
  VRG3DBase/src/VRG3DBaseApp.cpp
  VRG3DBase/src/ConfigVal.cpp
  VRG3DBase/src/GfxMgr.cpp
  VRG3DBase/src/StringUtils.cpp
  VRG3DBase/src/FsaHelper.cpp
  VRG3DBase/src/Fsa.cpp
  VRG3DBase/src/SMesh.cpp
  VRG3DBase/src/CovarianceMatrix.cpp
  VRG3DBase/src/Shadows.cpp
  VRG3DBase/src/EventMgr.cpp
  VRG3DBase/src/TexPerFrameSMesh.cpp
  VRG3DBase/src/LoadingScreen.cpp
  VRG3DBase/src/ViewerHCI.cpp
  VRG3DBase/src/TextFileReader.cpp

)

set (VRG3DBASEHEADERFILES 
 VRG3DBase/include/ConfigVal.H
 VRG3DBase/include/GfxMgr.H
 VRG3DBase/include/GfxMgrCallbacks.H
 VRG3DBase/include/StringUtils.H
 VRG3DBase/include/VRG3DBaseApp.h
 VRG3DBase/include/FsaHelper.H
 VRG3DBase/include/Fsa.H
 VRG3DBase/include/SMesh.H
 VRG3DBase/include/CovarianceMatrix.H
 VRG3DBase/include/Shadows.H
 VRG3DBase/include/EventMgr.H
 VRG3DBase/include/EventFilter.H
 VRG3DBase/include/TexPerFrameSMesh.H
 VRG3DBase/include/LoadingScreen.H
 VRG3DBase/include/ViewerHCI.H
 VRG3DBase/include/TextFileReader.H
)


# Define the target
add_library(${PROJECT_NAME} SHARED ${VRG3DHEADERFILES} ${VRG3DSOURCEFILES} ${VRG3DBASEHEADERFILES} ${VRG3DBASESOURCEFILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>                  # for headers when building
    $<INSTALL_INTERFACE:${INSTALL_PLUGININCLUDE_DEST}/${PROJECT_NAME}>  # for client in install mode
)

message(STATUS "Setting up filters")
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VRG3DSOURCEFILES} )#
#source_group("VRG3D" ${VRG3DHEADERFILES} ${VRG3DSOURCEFILES}  )
#foreach(source IN LISTS VRG3DSOURCEFILES)
#    get_filename_component(source_path "${source}" PATH)
#    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
#    source_group("${source_path_msvc}" FILES "${source}")
#endforeach()

#foreach(myinclude IN LISTS VRG3DHEADERFILES)
#    get_filename_component(source_path "${myinclude}" PATH)
#    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
#    source_group("${source_path_msvc}" FILES "${source}")
#endforeach()


# Setup dependency on libMinVR:
target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>  # for headers when building
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DEST}>              # for client in install mode
)
target_link_libraries(${PROJECT_NAME} PUBLIC MinVR)


# Setup dependency on G3D:
include(AutoBuildG3D)
AutoBuild_use_package_G3D(${PROJECT_NAME} PUBLIC)


# Setup dependency on GLEW (only needed on Windows):
#if (WIN32)
#  include(AutoBuildGLEW)
#  AutoBuild_use_package_GLEW(${PROJECT_NAME} PUBLIC)
#endif()

# G3D depencies


# Installation:
install(TARGETS ${PROJECT_NAME} EXPORT MinVRTargets COMPONENT ${PROJECT_NAME}
    LIBRARY DESTINATION ${INSTALL_PLUGINLIB_DEST}
    ARCHIVE DESTINATION ${INSTALL_PLUGINLIB_DEST}
    RUNTIME DESTINATION ${INSTALL_PLUGINDLL_DEST}
)
install(FILES ${HEADERFILES} DESTINATION ${INSTALL_PLUGININCLUDE_DEST}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})


# For better organization when using an IDE with folder structures:
source_group("Header Files" FILES ${HEADERFILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "plugins")
