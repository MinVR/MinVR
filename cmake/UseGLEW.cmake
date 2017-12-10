# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Tries to find a pre-installed version of GLEW on this system using find_package().
# Case 1: If found, then the script uses target_link_library() to link it to your target.
# Case 2: If not found and AUTOBUILD_DEPENDENCIES is ON, then the script immediately 
# downloads, builds, and installs the GLEW library to the location specifed by
# CMAKE_INSTALL_PREFIX. Then, it tries find_package() again and links to the newly install
# library.
# Case 3: If not found and AUTOBUILD_DEPENDENCIES is OFF, then the script exits with a
# fatal error.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the GLEW library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseGLEW)
#    UseGLEW(${PROJECT_NAME} PRIVATE)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html



macro(UseGLEW YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for GLEW library...")

    # Check to see if the library is already installed on the system
    # CMake ships with FindGLEW.cmake, which defines the GLEW::GLEW imported target
    # https://cmake.org/cmake/help/v3.9/module/FindGLEW.html
    find_package(GLEW)

    # Case 1: Already installed on the system
    if (${GLEW_FOUND})

        message(STATUS "Ok: GLEW Found.")
        message(STATUS "GLEW headers: ${GLEW_INCLUDE_DIR}")
	message(STATUS "GLEW libs: ${GLEW_LIBRARIES}")
	
	

    # Case 2: Download, build and install it now for the user, then try find_package() again
    elseif (AUTOBUILD_DEPENDENCIES)

        set(GLEW_AUTOBUILT TRUE)

        message(STATUS "Ok: AUTOBUILD_DEPENDENCIES is ON so GLEW will be downloaded into the external directory and built for you.")

        include(MinVRExternalProject)
        MinVRExternalProject_Download(
            GLEW
            URL https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.zip
        )

        MinVRExternalProject_BuildAndInstallNow(
            GLEW
            src
        )

        # Try find_package() again
        message(STATUS "Searching (again, right after autobuilding) for GLEW library...")
        find_package(GLEW)

        # We just intalled it to CMAKE_INSTALL_PREFIX, and the root CMakeLists.txt puts this in the
        # CMAKE_MODULE_PATH.  So, if we were not able to find the package now, then something is very wrong.
        if (NOT ${GLEW_FOUND})
            message(FATAL_ERROR "MinVR did an autobuild of the GLEW dependency, and it should now be installed at the prefix ${CMAKE_INSATALL_PREFIX}, but cmake is still unable to find it with find_package().")
        endif()


    # Case 3: The user does not want us to build it, so error out when not found.
    else()

        message(FATAL_ERROR "The GLEW library was not found on the system.  You can: (1) install GLEW yourself, (2)point cmake to an already-installed version of GLEW by adding the installation prefix of GLEW to the CMAKE_PREFIX_PATH environment variable, or (3) set AUTOBUILD_DEPENDENCIES to ON to have MinVR download, build, and install GLEW for you.")

    endif()


    # If we reach this point without an error, then one of the calls to find_package() was successful

    message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency GLEW::GLEW.")

    # No need to set include dirs; this uses the modern cmake imported targets, so they are set automatically
    if (TARGET GLEW::GLEW)
        target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} GLEW::GLEW)
    else()
        target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLEW_LIBRARIES})
	target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLEW_INCLUDE_DIR})
    endif()
    
    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_GLEW)

endmacro()

