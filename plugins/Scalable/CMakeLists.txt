# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Builds the MinVR_Scalable plugin as a shared library.
# This plugin requires a proprietary library from Scalable Display Technologies of 
# Cambridge, Massachusetts (scalabledisplay.com).

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)



# Plugin names must begin with "MinVR_"
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


# Define the target
add_library(${PROJECT_NAME} SHARED ${HEADERFILES} ${SOURCEFILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>                  # for headers when building
    $<INSTALL_INTERFACE:${INSTALL_PLUGININCLUDE_DEST}/${PROJECT_NAME}>  # for client in install mode
)


# Setup dependency on libMinVR:
target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>  # for headers when building
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DEST}>              # for client in install mode
)
target_link_libraries(${PROJECT_NAME} PUBLIC MinVR)


# Setup dependency on OpenGL:
include(AutoBuildOpenGL)
AutoBuild_use_package_OpenGL(${PROJECT_NAME} PUBLIC)


# Setup dependency on GLEW (not needed on OSX):
if (NOT APPLE)
  include(AutoBuildGLEW)
  AutoBuild_use_package_GLEW(${PROJECT_NAME} PUBLIC)
endif()


# Setup dependency on Scalable:
# This follows a different pattern because there is no option to auto-download-and-build
# the libScalable.
find_package(LibSCALABLE REQUIRED)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_EASYBLENDSDK_LINUX)
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBSCALABLE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBSCALABLE_ALL_LIBRARIES})


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

