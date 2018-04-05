# Author: Daniel F. Keefe
# Copyright 2017,2018, Daniel F. Keefe and Regents of the University of Minnesota
# All Rights Reserved.


# Usage:
# AutoBuild_use_package_OpenGL(
#    # The 1st argument is required.  It is the name of the target you wish to link this dependency to.
#    my-program 
#
#    # The 2nd argument is required.  It impacts how dependencies are propogated through CMake.  You can treat
#    # this dependency as an INTERFACE, PUBLIC, or PRIVATE.  See the following URL for details:
#    # https://cmake.org/cmake/help/latest/command/target_include_directories.html
#    PUBLIC 
# )
#
macro(AutoBuild_use_package_OpenGL YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "AutoBuild: Searching for package OpenGL...")

	# Check to see if the library is already installed on the system    
    # CMake ships with FindOpenGL.cmake and in CMake 3.9+ it defines
    # the imported targets OpenGL::GL and OpenGL::GLU.  Using these is
    # now the preferred way to link with OpenGL and all of its dependencies.
	# See https://cmake.org/cmake/help/v3.9/module/FindOpenGL.html
	find_package(OpenGL)

	if (NOT ${OPENGL_FOUND})
	    message(FATAL_ERROR "AutoBuild: OpenGL was not found on the system.  AutoBuild can auto-download 
        and build many dependencies for you, but not OpenGL. It should come pre-installed on your system.")
	endif()

    message(STATUS "AutoBuild: Ok. Found OpenGL.")
    message(STATUS "  OpenGL headers: ${OPENGL_INCLUDE_DIR}")
    message(STATUS "  OpenGL libs: ${OPENGL_LIBRARIES}")

    message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency OpenGL::GL.")
    target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} OpenGL::GL)

    if (${OPENGL_GLU_FOUND})
        message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency OpenGL::GLU.")
        target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} OpenGL::GLU)
    endif()

	target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_OPENGL)

endmacro()
