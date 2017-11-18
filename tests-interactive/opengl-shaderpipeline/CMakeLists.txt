# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

# Builds itest-opengl-shaderpipeline.

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)




# By convention, interactive tests begin with "itest-"
project(itest-opengl-shaderpipeline)


# Source:
set (SOURCEFILES 
  main.cpp
)
set (HEADERFILES
)
set (CONFIGFILES
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


# Create the executable:
include_directories(${MY_INC_DIRS})
link_directories(${MY_LIB_DIRS})
add_executable(${PROJECT_NAME} ${HEADERFILES} ${SOURCEFILES})
target_link_libraries(${PROJECT_NAME} ${MY_LIBS} ${LIBS_ALL})


# If we have dependencies that were not found pre-installed on the system
# and that will be built as part of the MinVR build process, then these
# must be listed as dependencies of the current project so that cmake
# builds them first.
if ("${GLEW_AUTOBUILT}" AND "WIN32")
    add_dependencies(${PROJECT_NAME} GLEW)
endif()


# Installation:
install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


# For better organization when using an IDE with folder structures:
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-interactive")
source_group("Header Files" FILES ${HEADERFILES})
set_source_files_properties(${CONFIGFILES} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Config Files" FILES ${CONFIGFILES})


