# This file is part of the MinVR cmake build system.
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Tries to find a pre-installed version of FreeGLUT on this system using find_package().
# Case 1: If found, then the script uses target_link_library() to link it to your target.
# Case 2: If not found and AUTOBUILD_DEPENDENCIES is ON, then the script immediately
# downloads, builds, and installs the FreeGLUT library to the location specifed by
# CMAKE_INSTALL_PREFIX. Then, it tries find_package() again and links to the newly install
# library.
# Case 3: If not found and AUTOBUILD_DEPENDENCIES is OFF, then the script exits with a
# fatal error.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the FreeGLUT library (typical with something like add_executable(${PROJECT_NAME} ...)
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseFreeGLUT)
#    UseFreeGLUT(${PROJECT_NAME} PRIVATE)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here:
# https://cmake.org/cmake/help/latest/command/target_include_directories.html




# NOTE November 2017: FreeGLUT seems to not build on OSX 10.10 at the moment -- looking
# at the forums, this is a known bug in FreeGLUT, not a problem with this build system.


macro(UseFreeGLUT YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for FreeGLUT library...")

    # Check to see if the library is already installed on the system
    # CMake provides FindGLUT, which finds either glut and freeglut, but MinVR only
    # works with freeglut, so the call below is using our own config/FindFreeGLUT.cmake
    # script.
    find_package(FreeGLUT)

    # Case 1: Already installed on the system
    if (${FREEGLUT_FOUND})

        message(STATUS "Ok: FreeGLUT Found.")
        message(STATUS "FreeGLUT headers: ${FREEGLUT_INCLUDE_DIR}")
        message(STATUS "FreeGLUT libs: ${FREEGLUT_LIBRARIES}")


    # Case 2: Download, build and install it now for the user, then try find_package() again
    elseif (AUTOBUILD_DEPENDENCIES)

        set(FreeGLUT_AUTOBUILT TRUE)

        message(STATUS "Ok: AUTOBUILD_DEPENDENCIES is ON so FreeGLUT will be downloaded into the external directory and built for you.")

        include(MinVRExternalProject)
        MinVRExternalProject_Download(
            FreeGLUT
            SVN_REPOSITORY https://svn.code.sf.net/p/freeglut/code/trunk/freeglut/freeglut
            SVN_TRUST_CERT 1
        )

        MinVRExternalProject_BuildAndInstallNow(
            FreeGLUT
            src
        )

        # Try find_package() again
        message(STATUS "Searching (again, right after autobuilding) for FreeGLUT library...")
        find_package(FreeGLUT)

        # We just intalled it to CMAKE_INSTALL_PREFIX, and the root CMakeLists.txt puts this in the
        # CMAKE_MODULE_PATH.  So, if we were not able to find the package now, then something is very wrong.
        if (NOT ${FREEGLUT_FOUND})
            message(FATAL_ERROR "MinVR did an autobuild of the FreeGLUT dependency, and it should now be installed at the prefix ${CMAKE_INSTALL_PREFIX}, but cmake is still unable to find it with find_package().")
        endif()

        message(STATUS "FreeGLUT headers: ${FREEGLUT_INCLUDE_DIR}")
        message(STATUS "FreeGLUT libs: ${FREEGLUT_LIBRARIES}")


    # Case 3: The user does not want us to build it, so error out when not found.
    else()

        message(FATAL_ERROR "The FreeGLUT library was not found on the system.  You can: (1) install FreeGLUT yourself, (2)point cmake to an already-installed version of FreeGLUT by adding the installation prefix of FreeGLUT to the CMAKE_PREFIX_PATH environment variable, or (3) set AUTOBUILD_DEPENDENCIES to ON to have MinVR download, build, and install FreeGLUT for you.")

    endif()


    # If we reach this point without an error, then one of the calls to find_package() was successful

    message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency FreeGLUT.")
    target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${FREEGLUT_LIBRARIES})
    target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${FREEGLUT_INCLUDE_DIR})
    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_FREEGLUT)

endmacro()

