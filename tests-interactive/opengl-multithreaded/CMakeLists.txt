# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

# Builds itest-opengl-multithreaded.

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)


# By convention, interactive tests begin with "itest-"
project(itest-opengl-multithreaded)


# Source:
set (SOURCEFILES 
  main.cpp
  MyAppModel.cpp
  MyAppController.cpp
  MyAppView.cpp
  MyAppSharedContext.cpp
)
set (HEADERFILES
  MyAppModel.h
  MyAppController.h
  MyAppView.h
  MyAppSharedContext.h
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


# Setup dependency on GLEW (not needed on OSX):
if (NOT APPLE)
    include(AutoBuildGLEW)
    AutoBuild_use_package_GLEW(${PROJECT_NAME} PUBLIC)
endif()

# Setup dependency on C++-11:
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -std=c++11)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  target_compile_options(${PROJECT_NAME} PRIVATE -std=c++11 -stdlib=libc++)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7")
    target_compile_options(${PROJECT_NAME} PRIVATE -std=gnu++0x)
  else()
    target_compile_options(${PROJECT_NAME} PRIVATE -std=c++11)
  endif()
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




