# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Tries to find a pre-installed version of VRPN on this system using find_package().
# Case 1: If found, then the script uses target_link_library() to link it to your target.
# Case 2: If not found and AUTOBUILD_DEPENDENCIES is ON, then the script immediately 
# downloads, builds, and installs the VRPN library to the location specifed by
# CMAKE_INSTALL_PREFIX. Then, it tries find_package() again and links to the newly install
# library.
# Case 3: If not found and AUTOBUILD_DEPENDENCIES is OFF, then the script exits with a
# fatal error.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the VRPN library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseVRPN)
#    UseVRPN(${PROJECT_NAME} PRIVATE)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html



macro(UseVRPN YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for VRPN library...")

    # Check to see if the library is already installed on the system
    # CMake does not provide a FindVRPN.cmake, and VRPN does not provide a config package.
    # So, the call below is using our own cmake/Modules/FindVRPN.cmake script.
    find_package(VRPN)

    # Case 1: Already installed on the system
    if (${VRPN_FOUND})

        message(STATUS "Ok: VRPN Found.")


    # Case 2: Download, build and install it now for the user, then try find_package() again
    elseif (AUTOBUILD_DEPENDENCIES)

        set(VRPN_AUTOBUILT TRUE)

        message(STATUS "Ok: AUTOBUILD_DEPENDENCIES is ON so VRPN will be downloaded into the external directory and built for you.")

        include(MinVRExternalProject)
        MinVRExternalProject_Download(
            VRPN
            URL https://github.com/vrpn/vrpn/releases/download/v07.33/vrpn_07_33.zip
        )

        set(VRPN_CONFIG_OPTIONS
            -DVRPN_BUILD_DIRECTSHOW_VIDEO_SERVER=OFF 
            -DVRPN_BUILD_HID_GUI=OFF
            -DVRPN_BUILD_JAVA=OFF
            -DVRPN_BUILD_PYTHON=OFF
            -DVRPN_USE_DIRECTINPUT=OFF
            -DVRPN_USE_DIRECTSHOW=OFF
        )

        if (MSVC)
            set(VRPN_CONFIG_OPTIONS ${VRPN_CONFIG_OPTIONS}
                -DVRPN_USE_HID=ON
                -DVRPN_HID_DEBUGGING=ON
                -DVRPN_USE_LOCAL_HIDAPI=ON
            )
        endif()

        MinVRExternalProject_BuildAndInstallNow(
            VRPN
            src
            ${VRPN_CONFIG_OPTIONS}
        )

        # Try find_package() again
        message(STATUS "Searching (again, right after autobuilding) for VRPN library...")
        find_package(VRPN)

        # We just intalled it to CMAKE_INSTALL_PREFIX, and the root CMakeLists.txt puts this in the
        # CMAKE_MODULE_PATH.  So, if we were not able to find the package now, then something is very wrong.
        if (NOT ${VRPN_FOUND})
            message(FATAL_ERROR "MinVR did an autobuild of the VRPN dependency, and it should now be installed at the prefix ${CMAKE_INSATALL_PREFIX}, but cmake is still unable to find it with find_package().")
        endif()


    # Case 3: The user does not want us to build it, so error out when not found.
    else()

        message(FATAL_ERROR "The VRPN library was not found on the system.  You can: (1) install VRPN yourself, (2)point cmake to an already-installed version of VRPN by adding the installation prefix of VRPN to the CMAKE_PREFIX_PATH environment variable, or (3) set AUTOBUILD_DEPENDENCIES to ON to have MinVR download, build, and install VRPN for you.")

    endif()


    # If we reach this point without an error, then one of the calls to find_package() was successful

    message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency VRPN.")
    target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${VRPN_LIBRARIES})
    target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${VRPN_INCLUDE_DIR})
    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_VRPN)

endmacro()
