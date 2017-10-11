
set(VRPlugins_checkout_dir ${PROJECT_SOURCE_DIR}/plugins)
set(VRPlugins_build_dir ${CMAKE_BINARY_DIR}/plugins)
set(VRPlugins_stamp_dir ${VRPlugins_build_dir}/stamp)
set(VRPlugins_tmp_dir ${VRPlugins_build_dir}/tmp)
make_directory(${VRPlugins_checkout_dir})
set(VRPlugins_install_dir "${CMAKE_INSTALL_PREFIX}/plugins")
set(VRPlugins_CMAKE_ARGS -DMinVR_INCLUDE_DIRS=${PROJECT_SOURCE_DIR}/.. -DMinVR_BINARY_DIRS=${CMAKE_BINARY_DIR}/lib -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(MinVRExtensions_CMAKE_ARGS ${VRPlugins_CMAKE_ARGS} -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD="c++11" -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY="libc++")
endif()

include(ExternalProject)

#------------------------------------------
# Specific preprocessor defines
#------------------------------------------

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
	set(LIBS_ALL ${LIBS_ALL} ${COCOA_LIB} ${IOKIT_LIB} ${CORE_FOUNDATION_FRAMEWORK} ${CORE_VIDEO_FRAMEWORK})
	message(STATUS "Core video framework: ${CORE_VIDEO_FRAMEWORK}")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(Threads)
	find_package(X11)
	set(LIBS_ALL ${LIBS_ALL} ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xi m dl ${X11_LIBRARIES})
endif()

#------------------------------------------
# Set output directories to lib, and bin
#------------------------------------------

make_directory(${CMAKE_BINARY_DIR}/lib)
make_directory(${CMAKE_BINARY_DIR}/bin)

set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")
set(CMAKE_MINSIZEREL_POSTFIX "s")

#set the build postfix extension according to the current configuration
if (CMAKE_BUILD_TYPE MATCHES "Release")
	set(CMAKE_BUILD_POSTFIX "${CMAKE_RELEASE_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
	set(CMAKE_BUILD_POSTFIX "${CMAKE_MINSIZEREL_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
	set(CMAKE_BUILD_POSTFIX "${CMAKE_RELWITHDEBINFO_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
	set(CMAKE_BUILD_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
else()
	set(CMAKE_BUILD_POSTFIX "")
endif()

message(STATUS "Considering plugins...")

#------------------OpenGL-----------------------------
option(MINVR_OPENGL_PLUGIN "If enabled, OpenGL plugin will be installed" OFF)
if (MINVR_OPENGL_PLUGIN)
  message(STATUS "Installing the OpenGL plugin.")
	add_subdirectory (OpenGL)
else()
  message(STATUS "NOT installing the OpenGL plugin.  Choose it with '-DMINVR_OPENGL_PLUGIN=ON'")
endif()

#------------------GLFW-----------------------------
option(MINVR_GLFW_PLUGIN "If enabled, GLFW plugin will be installed" OFF)
if (MINVR_GLFW_PLUGIN)
  message(STATUS "Installing the GLFW plugin.")
	add_subdirectory (GLFW)
else()
  message(STATUS "NOT installing the GLFW plugin.  Choose it with '-DMINVR_GLFW_PLUGIN=ON'")
endif()

#------------------VRPN-----------------------------
option(MINVR_VRPN_PLUGIN "If enabled, VRPN plugin will be installed" OFF)
if (MINVR_VRPN_PLUGIN)
  message(STATUS "Installing the VRPN plugin.")
	add_subdirectory (VRPN)
else()
  message(STATUS "NOT installing the VRPN plugin.  Choose it with '-DMINVR_VRPN_PLUGIN=ON'")
endif()

#------------------Python-----------------------------
option(MINVR_PYTHON_PLUGIN "If enabled, Python plugin will be installed" OFF)
if (MINVR_PYTHON_PLUGIN)
  message(STATUS "Installing the Python plugin.")
	add_subdirectory (Python)
else()
  message(STATUS "NOT installing the Python plugin.  Choose it with '-DMINVR_PYTHON_PLUGIN=ON'")
endif()

#------------------Threading-----------------------------
option(MINVR_THREADING_PLUGIN "If enabled, Threading plugin will be installed" OFF)
if (MINVR_THREADING_PLUGIN)
  message(STATUS "Installing the Threading plugin.")
	add_subdirectory (Threading)
else()
  message(STATUS "NOT installing the Threading plugin.  Choose it with '-DMINVR_THREADING_PLUGIN=ON'")
endif()

#------------------TUIO-----------------------------
option(MINVR_TUIO_PLUGIN "If enabled, TUIO plugin will be installed" OFF)
if (MINVR_TUIO_PLUGIN)
  message(STATUS "Installing the TUIO plugin.")
	add_subdirectory (TUIO)
else()
  message(STATUS "NOT installing the TUIO plugin.  Choose it with '-DMINVR_TUIO_PLUGIN=ON'")
endif()

#------------------OpenVR-----------------------------
option(MINVR_OPENVR_PLUGIN "If enabled, OpenVR plugin will be installed" OFF)
if (MINVR_OPENVR_PLUGIN)
  message(STATUS "Installing the OpenVR plugin.")
	add_subdirectory (OpenVR)
else()
  message(STATUS "NOT installing the OpenVR plugin.  Choose it with '-DMINVR_OPENVR_PLUGIN=ON'")
endif()

#------------------FreeGlut-----------------------------
option(MINVR_FREEGLUT_PLUGIN "If enabled, FreeGlut plugin will be installed" OFF)
if (MINVR_FREEGLUT_PLUGIN)
  message(STATUS "Installing the FREEGLUT plugin.")
	add_subdirectory (FREEGLUT)
else()
  message(STATUS "NOT installing the FREEGLUT plugin.  Choose it with '-DMINVR_FREEGLUT_PLUGIN=ON'")
endif()

#------------------Scalable-----------------------------
option(MINVR_SCALABLE_PLUGIN "If enabled, SCALABLE plugin will be installed. Requires proprietary software from scalabledisplay.com" OFF)
if (MINVR_SCALABLE_PLUGIN)
  message(STATUS "Installing the Scalable plugin.")
	add_subdirectory (Scalable)
else()
  message(STATUS "NOT installing the Scalable plugin.  Choose it with '-DMINVR_SCALABLE_PLUGIN=ON'")
endif()

