# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

# Builds itest-opengl-shaderpipeline-with-api.

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)


# By convention, interactive tests begin with "itest-"
project(itest-gamepad-opengl-shaderpipeline-with-api)


# Source:
set (SOURCEFILES 
  main.cpp
  
)
set (HEADERFILES
  
)
set (CONFIGFILES
)


# Define the target
add_executable(${PROJECT_NAME} ${HEADERFILES} ${SOURCEFILES})


# Setup dependency on libMinVR:
target_include_directories(${PROJECT_NAME} PUBLIC ../../src)
target_link_libraries(${PROJECT_NAME} PUBLIC MinVR)


# Setup dependency on OpenGL:
include(AutoBuildOpenGL)
AutoBuild_use_package_OpenGL(${PROJECT_NAME} PUBLIC)

include(AutoBuildGainput)
AutoBuild_use_package_Gainput(${PROJECT_NAME} PUBLIC)

if(${CMAKE_BUILD_TYPE} EQUAL Debug )
 set(GAINPUT_LIB "MinVR_Gainputd.lib")
else()
 set(GAINPUT_LIB "MinVR_Gainput.lib")
endif()

set(MinVR_GAINPUT_LIB ${CMAKE_CURRENT_BINARY_DIR}/../../lib/${GAINPUT_LIB})
message(STATUS "MinVR_GAINPUT_LIB is ${MinVR_GAINPUT_LIB}") 

if(EXISTS ${MinVR_GAINPUT_LIB})
   message(STATUS "MinVR_GAINPUT_LIB found in ${MinVR_GAINPUT_LIB}") 
   target_link_libraries(${PROJECT_NAME} PUBLIC ${MinVR_GAINPUT_LIB})
else()
    message(WARNING "YOU MIGHT HAVE TO LINK MinVR_Gainput.lib TO YOUR PROJECT MANUALLY")
endif()

# Setup dependency on GLEW (not needed on OSX):
if (NOT APPLE)
  include(AutoBuildGLEW)
  AutoBuild_use_package_GLEW(${PROJECT_NAME} PUBLIC)
endif()


# Installation:
install( TARGETS ${PROJECT_NAME}
         RUNTIME DESTINATION ${INSTALL_BIN_DEST}
         COMPONENT ITests)



# For better organization when using an IDE with folder structures:
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-interactive")
source_group("Header Files" FILES ${HEADERFILES})
set_source_files_properties(${CONFIGFILES} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Config Files" FILES ${CONFIGFILES})


