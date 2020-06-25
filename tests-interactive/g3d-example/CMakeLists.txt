# Template for creating a CMakeLists.txt for new interactive tests / other apps that are
# built within the MinVR tree -- copy this, replace XXX and YYY, and follow the example of 
# OpenGL and GLEW below if your app depends on an external library.
# -----------------------------------------------------------------------------------------
# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

# Builds itest-XXX.

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)


# By convention, interactive tests begin with "itest-"
project(itest-G3D-example)


# Source:
set (SOURCEFILES 
  main.cpp
)
set (HEADERFILES
)
set (CONFIGFILES
  desktop-oldopengl.minvr
)


# Define the target
add_executable(${PROJECT_NAME} ${HEADERFILES} ${SOURCEFILES})

get_property(VRG3D_INCLUDE_DIR GLOBAL PROPERTY VRG3D_INCLUDE_DIR_PROPERTY)
message(STATUS "VRG3D_PLUGIN_DIR is " ${VRG3D_INCLUDE_DIR})
message(STATUS "PROJECT_NAME is " ${PROJECT_NAME})

# Setup dependency on libMinVR:
target_include_directories(${PROJECT_NAME} PUBLIC ${VRG3D_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC MinVR)

message(STATUS "HELLO WORLD is ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR is ${CMAKE_CURRENT_BINARY_DIR}")
set(MinVR_G3D_LIB ${CMAKE_CURRENT_BINARY_DIR}/../../lib/MinVR_G3Dd.lib)
message(STATUS "MinVR_G3Dd is ${MinVR_G3D_LIB}") 

if(EXISTS ${MinVR_G3D_LIB})
   message(STATUS "MinVR_G3Dd found in ${MinVR_G3D_LIB}") 
   target_link_libraries(${PROJECT_NAME} PUBLIC ${MinVR_G3D_LIB})
else()
    message(WARNING "YOU MIGHT HAVE TO LINK MinVR_G3Dd.lib IN YOUR PROJECT MANUALLY")
endif()

# Setup dependency on G3D:
include(AutoBuildG3D)
AutoBuild_use_package_G3D(${PROJECT_NAME} PUBLIC)



# Installation:
install( TARGETS ${PROJECT_NAME}
         RUNTIME DESTINATION ${INSTALL_BIN_DEST}
         COMPONENT ITests)


# For better organization when using an IDE with folder structures:
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-interactive")
source_group("Header Files" FILES ${HEADERFILES})
set_source_files_properties(${CONFIGFILES} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Config Files" FILES ${CONFIGFILES})
