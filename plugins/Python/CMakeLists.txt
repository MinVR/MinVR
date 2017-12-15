# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Builds the MinVR_Python plugin as a shared library.

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)




# Plugin names must begin with "MinVR_"
project(MinVR_Python)


# Source:
set (SOURCEFILES 
  src/VRPythonPlugin.cpp
  src/VRMainPython.cpp
  src/VRDataIndexPython.cpp
)
set (HEADERFILES
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
install(TARGETS ${PROJECT_NAME} EXPORT MinVRTargets COMPONENT ${PROJECT_NAME}
    LIBRARY DESTINATION ${INSTALL_PLUGINLIB_DEST}
    ARCHIVE DESTINATION ${INSTALL_PLUGINLIB_DEST}
    RUNTIME DESTINATION ${INSTALL_PLUGINDLL_DEST}
)
install(FILES ${HEADERFILES} DESTINATION ${INSTALL_PLUGININCLUDE_DEST}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})


# For better organization when using an IDE with folder structures:
source_group("Header Files" FILES ${HEADERFILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "plugins")
