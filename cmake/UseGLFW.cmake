# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Tries to find a pre-installed version of GLFW on this system using find_package().
# Case 1: If found, then the script uses target_link_library() to link it to your target.
# Case 2: If not found and AUTOBUILD_DEPENDENCIES is ON, then the script immediately 
# downloads, builds, and installs the GLFW library to the location specifed by
# CMAKE_INSTALL_PREFIX. Then, it tries find_package() again and links to the newly install
# library.
# Case 3: If not found and AUTOBUILD_DEPENDENCIES is OFF, then the script exits with a
# fatal error.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the GLFW library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseGLFW)
#    UseGLFW(${PROJECT_NAME} PRIVATE)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html



macro(UseGLFW YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for GLFW library...")

    # Check to see if the library is already installed on the system
    # GLFW installs a config package to <prefix>/lib/cmake/glfw/glfwConfig.cmake but it
    # does not define targets properly, so we use our own cmake/Modules/FindGLFW.cmake
    # script -- the MODULE flag below forces the use of a Find* script rather than using
    # the supplied *Config package.

    find_package(GLFW MODULE)

    # Case 1: Already installed on the system
    if (${GLFW_FOUND})

        message(STATUS "Ok: GLFW Found.")


    # Case 2: Download, build and install it now for the user, then try find_package() again
    elseif (AUTOBUILD_DEPENDENCIES)

        set(GLFW_AUTOBUILT TRUE)

        message(STATUS "Ok: AUTOBUILD_DEPENDENCIES is ON so GLFW will be downloaded into the external directory and built for you.")

        include(MinVRExternalProject)
        MinVRExternalProject_Download(
            GLFW
            GIT_REPOSITORY https://github.com/ivlab/glfw.git
            GIT_TAG gpu-affinity
        )

        MinVRExternalProject_BuildAndInstallNow(
            GLFW
            src
        )

        # Try find_package() again
        message(STATUS "Searching (again, right after autobuilding) for GLFW library...")
        find_package(GLFW MODULE)

        # We just intalled it to CMAKE_INSTALL_PREFIX, and the root CMakeLists.txt puts this in the
        # CMAKE_MODULE_PATH.  So, if we were not able to find the package now, then something is very wrong.
        if (NOT ${GLFW_FOUND})
            message(FATAL_ERROR "MinVR did an autobuild of the GLFW dependency, and it should now be installed at the prefix ${CMAKE_INSTALL_PREFIX}, but cmake is still unable to find it with find_package().")
        endif()


    # Case 3: The user does not want us to build it, so error out when not found.
    else()

        message(FATAL_ERROR "The GLFW library was not found on the system.  You can: (1) install GLFW yourself, (2)point cmake to an already-installed version of GLFW by adding the installation prefix of GLFW to the CMAKE_PREFIX_PATH environment variable, or (3) set AUTOBUILD_DEPENDENCIES to ON to have MinVR download, build, and install GLFW for you.")

    endif()


    # If we reach this point without an error, then one of the calls to find_package() was successful

    message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency GLFW.")

    # GLFW is exporting a cmake config package, but it doesn't seem to be setup correctly to work
    # with imported targets.  So, we just add the libraries and includes manually here based on
    # the cmake variables it sets.

    target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLFW_INCLUDE_DIR})
    target_link_libraries(${YOUR_TARGET} 
        ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLFW_LIBRARIES}
        ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLFW_cocoa_LIBRARY}
        ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLFW_corevideo_LIBRARY}
        ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLFW_iokit_LIBRARY}
    )

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        find_package(Threads)
        find_package(X11)
        target_link_libraries(${YOUR_TARGET} 
            ${INTERFACE_PUBLIC_OR_PRIVATE} rt
            ${INTERFACE_PUBLIC_OR_PRIVATE} Xrandr
            ${INTERFACE_PUBLIC_OR_PRIVATE} Xxf86vm
            ${INTERFACE_PUBLIC_OR_PRIVATE} Xi
            ${INTERFACE_PUBLIC_OR_PRIVATE} m
            ${INTERFACE_PUBLIC_OR_PRIVATE} ${X11_LIBRARIES}
        )
    endif()

    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_GLFW)

endmacro()

