
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

option(BASE_PLUGINS "If enabled, Base plugins will be installed" ON)

#------------------OpenGL-----------------------------
option(MINVR_OPENGL_PLUGIN "If enabled, OpenGL plugin will be installed" OFF)
if (MINVR_OPENGL_PLUGIN OR BASE_PLUGINS)
	add_subdirectory (OpenGL)
endif()

#------------------GLFW-----------------------------
option(MINVR_GLFW_PLUGIN "If enabled, GLFW plugin will be installed" OFF)
if (MINVR_GLFW_PLUGIN OR BASE_PLUGINS)
	add_subdirectory (GLFW)
endif()

#------------------VRPN-----------------------------
option(MINVR_VRPN_PLUGIN "If enabled, VRPN plugin will be installed" OFF)
if (MINVR_VRPN_PLUGIN)
	add_subdirectory (VRPN)
endif()

#------------------Python-----------------------------
option(MINVR_PYTHON_PLUGIN "If enabled, Python plugin will be installed" OFF)
if (MINVR_PYTHON_PLUGIN OR BASE_PLUGINS)
	add_subdirectory (Python)
endif()

#------------------Threading-----------------------------
option(MINVR_THREADING_PLUGIN "If enabled, Threading plugin will be installed" OFF)
if (MINVR_THREADING_PLUGIN)
	add_subdirectory (Threading)
endif()

#------------------TUIO-----------------------------
option(MINVR_TUIO_PLUGIN "If enabled, TUIO plugin will be installed" OFF)
if (MINVR_TUIO_PLUGIN)
	add_subdirectory (TUIO)
endif()

#------------------OpenVR-----------------------------
option(MINVR_OPENVR_PLUGIN "If enabled, OpenVR plugin will be installed" OFF)
if (MINVR_OPENVR_PLUGIN)
	add_subdirectory (OpenVR)
endif()

#------------------FreeGlut-----------------------------
option(MINVR_FREEGLUT_PLUGIN "If enabled, FreeGlut plugin will be installed" OFF)
if (MINVR_FREEGLUT_PLUGIN)
	add_subdirectory (FREEGLUT)
endif()

#------------------Scalable-----------------------------
option(MINVR_SCALABLE_PLUGIN "If enabled, SCALABLE plugin will be installed. Requires proprietary software from scalabledisplay.com" OFF)
if (MINVR_SCALABLE_PLUGIN)
	add_subdirectory (Scalable)
endif()

