# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Builds the MinVR_Threading plugin as a shared library.

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)




# Plugin names must begin with "MinVR_"
project(MinVR_Threading)


# Source:
set (SOURCEFILES 
  src/VRRenderThread.cpp
  src/VRThreadGroup.cpp
  src/VRThreadGroupNode.cpp
  src/VRThreadingPlugin.cpp
)
set (HEADERFILES
  src/VRRenderThread.h
  src/VRThreadGroup.h
  src/VRThreadGroupNode.h
  src/VRThread.h
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
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/MinVR${MINVR_VERSION_STR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/MinVR${MINVR_VERSION_STR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
install(FILES ${HEADERFILES} DESTINATION include/MinVR${MINVR_VERSION_STR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})


# For better organization when using an IDE with folder structures:
source_group("Header Files" FILES ${HEADERFILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "plugins")


