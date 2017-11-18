# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Either finds a pre-installed version or complains.
#
# Usage: In your CMakeLists.txt:
# 1. Somewhere before your add_executable() or add_library() line, add the following:
#        include(UseOpenGL)
#        UseOpenGL()
#
# This Use*() macro is a bit different than the others since it's not possible to download
# and build OpenGL -- this is a package that comes pre-installed with your OS and/or 
# graphics drivers.


macro(UseOpenGL)

    message(STATUS "Searching for OpenGL...")

	# Check to see if the library is already installed on the system
	find_package(OpenGL)

	# If not, either autobuild it or provide an informative error message
	if (NOT ${OpenGL_FOUND})

	    message(FATAL_ERROR "OpenGL was not found on the system.  MinVR can auto-download and build many dependencies for you, but not OpenGL. It should come pre-installed on your system.")

	endif()

    message(STATUS "Ok: OpenGL Found.")

	add_definitions(-DUSE_OPENGL)

    message(STATUS "Adding build flags for OpenGL programs.")

	set(OPENGL_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
	set(OPENGL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
	
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
		list(APPEND OPENGL_LIBRARIES ${COCOA_LIB} ${IOKIT_LIB} ${CORE_FOUNDATION_FRAMEWORK} ${CORE_VIDEO_FRAMEWORK})
	endif()

	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		find_package(Threads)
		find_package(X11)
		list(APPEND OPENGL_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xinerama Xcursor Xi m dl ${X11_LIBRARIES})
	endif()

endmacro()
