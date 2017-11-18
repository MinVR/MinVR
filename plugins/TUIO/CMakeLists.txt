# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Builds the MinVR_TUIO plugin as a shared library.

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)




# By convention, plugin names begin with "MinVR_"
project(MinVR_TUIO)


# Source:
set (SOURCEFILES 
  src/VRTUIOPlugin.cpp
  src/VRTUIODevice.cpp
)
set (HEADERFILES
  src/VRTUIODevice.h
)


# Setup dependency on libMinVR:
list(APPEND MY_INC_DIRS src)
list(APPEND MY_LIB_DIRS ${CMAKE_BINARY_DIR})
list(APPEND MY_LIBS MinVR)


# Setup dependency on TUIO:
include(UseTUIO)
UseTUIO()
list(APPEND MY_INC_DIRS ${TUIO_INCLUDE_DIR})
list(APPEND MY_LIB_DIRS ${TUIO_LIBRARY_DIR})
list(APPEND MY_LIBS ${TUIO_LIBRARIES})


# Create the shared library:
include_directories(${MY_INC_DIRS})
link_directories(${MY_LIB_DIRS})
add_library(${PROJECT_NAME} SHARED ${HEADERFILES} ${SOURCEFILES})
target_link_libraries(${PROJECT_NAME} ${MY_LIBS} ${LIBS_ALL})


# If we have dependencies that were not found pre-installed on the system
# and that will be built as part of the MinVR build process, then these
# must be listed as dependencies of the current project so that cmake
# builds them first.
if (${TUIO_AUTOBUILT})
    add_dependencies(${PROJECT_NAME} TUIO)
endif()


# Installation:
install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)


# For better organization when using an IDE with folder structures:
source_group("Header Files" FILES ${HEADERFILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "plugins")


