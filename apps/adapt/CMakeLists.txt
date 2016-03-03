cmake_minimum_required (VERSION 2.6)

project (adapt)

#---------------- OS specific settings -------------
# Windows Section #
if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${WINDOWS_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	find_library(COCOA_LIB Cocoa)
	find_library(IOKIT_LIB IOKit)
	find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
    	find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
	set(ALL_LIBS ${ALL_LIBS} ${COCOA_LIB} ${IOKIT_LIB} ${CORE_FOUNDATION_FRAMEWORK} ${CORE_VIDEO_FRAMEWORK})
	message(STATUS "${CORE_VIDEO_FRAMEWORK}")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(Threads)
	find_package(X11)
	set(ALL_LIBS ${ALL_LIBS} ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xi m dl ${X11_LIBRARIES})
endif()

#------------------------------------------

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(
  /usr/local/include
  /usr/local/include/GLFW
  /usr/local/X11/include
  /opt/X11/include
  )

set(ALL_LIBS
	${OPENGL_LIBRARY}
  ${GLUT_LIBRARY}
  )

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
  # This refers to gluLookAt and gluPerspective, at least under OS X
  -Wno-deprecated-declarations
  )

add_executable (adaptee
  adaptee.cpp
  )

target_link_libraries (adaptee ${ALL_LIBS})

add_executable (adapted
  adapted.cpp
  )

target_link_libraries (adapted VRMain VRPlugin VRDisplay VRIndex VRMath VRNet ${ALL_LIBS} dl)

add_executable (adapted_server
  adapted_server.cpp
  )

target_link_libraries (adapted_server VRMain VRPlugin VRDisplay VRIndex VRMath VRNet ${ALL_LIBS})

#----------------------plugins------------------------------

#add_definitions(-DPLUGINPATH="${CMAKE_BINARY_DIR}/plugins")
#add_executable ( adapted_plugins adapted_plugins.cpp )
#target_link_libraries(adapted_plugins VRMain VRPlugin VRDisplay VRIndex VRMath VRNet ${ALL_LIBS} dl)

