# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Tries to find a pre-installed version of OpenVR on this system using find_package().
# Case 1: If found, then the script uses target_link_library() to link it to your target.
# Case 2: If not found and AUTOBUILD_DEPENDENCIES is ON, then the script immediately 
# downloads, builds, and installs the OpenVR library to the location specifed by
# CMAKE_INSTALL_PREFIX. Then, it tries find_package() again and links to the newly install
# library.
# Case 3: If not found and AUTOBUILD_DEPENDENCIES is OFF, then the script exits with a
# fatal error.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the OpenVR library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseOpenVR)
#    UseOpenVR(${PROJECT_NAME} PRIVATE)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html



macro(UseOpenVR YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for OpenVR library...")

    # Check to see if the library is already installed on the system
    # CMake does not provide a FindOpenVR.cmake, and OpenVR does not provide a config package.
    # So, the call below is using our own cmake/Modules/FindOpenVR.cmake script.
    find_package(OpenVR)

    # Case 1: Already installed on the system
    if (${OpenVR_FOUND})

        message(STATUS "Ok: OpenVR Found.")
        message(STATUS "OpenVR headers: ${OPENVR_INCLUDE_DIR}")
        message(STATUS "OpenVR libs: ${OPENVR_LIBRARIES}")


    # Case 2: Download, build and install it now for the user, then try find_package() again
    elseif (AUTOBUILD_DEPENDENCIES)

        set(OpenVR_AUTOBUILT TRUE)

        message(STATUS "Ok: AUTOBUILD_DEPENDENCIES is ON so OpenVR will be downloaded into the external directory and built for you.")

        include(MinVRExternalProject)
        MinVRExternalProject_Download(
            OpenVR
            GIT_REPOSITORY https://github.com/ivlab/openvr
            GIT_TAG master
        )

        MinVRExternalProject_BuildAndInstallNow(
            OpenVR
            src
        )

        # OpenVR's install target seems to only install the lib.  So, install headers manually here.
        file(INSTALL ${EXTERNAL_DIR}/OpenVR/src/headers/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/openvr)

        # Try find_package() again
        message(STATUS "Searching (again, right after autobuilding) for OpenVR library...")
        find_package(OpenVR)

        # We just intalled it to CMAKE_INSTALL_PREFIX, and the root CMakeLists.txt puts this in the
        # CMAKE_MODULE_PATH.  So, if we were not able to find the package now, then something is very wrong.
        if (NOT ${OPENVR_FOUND})
            message(FATAL_ERROR "MinVR did an autobuild of the OpenVR dependency, and it should now be installed at the prefix ${CMAKE_INSTALL_PREFIX}, but cmake is still unable to find it with find_package().")
        endif()

        message(STATUS "OpenVR headers: ${OPENVR_INCLUDE_DIR}")
        message(STATUS "OpenVR libs: ${OPENVR_LIBRARIES}")


    # Case 3: The user does not want us to build it, so error out when not found.
    else()

        message(FATAL_ERROR "The OpenVR library was not found on the system.  You can: (1) install OpenVR yourself, (2)point cmake to an already-installed version of OpenVR by adding the installation prefix of OpenVR to the CMAKE_PREFIX_PATH environment variable, or (3) set AUTOBUILD_DEPENDENCIES to ON to have MinVR download, build, and install OpenVR for you.")

    endif()


    # If we reach this point without an error, then one of the calls to find_package() was successful

    message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency OpenVR.")

    # This uses "old-style" cmake, rather than importing a target from a cmake config file.
    # We use FindOpenVR to set OPENVR_LIBRARIES AND OPENVR_INCLUDE_DIR and then we add 
    # these manually to our target.

    target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${OPENVR_LIBRARIES})
    target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${OPENVR_INCLUDE_DIR})
    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_OPENVR)


    message(STATUS "Adding OpenVR's OS-specific build flags and dependencies to target ${YOUR_TARGET}.")
    if (MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        # Tell MSVC to use main instead of WinMain for Windows subsystem executables
        set_target_properties(${WINDOWS_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        find_library(COCOA_LIB Cocoa)
        target_link_libraries(${YOUR_TARGET} PRIVATE ${COCOA_LIB})
        find_library(IOKIT_LIB IOKit)
        target_link_libraries(${YOUR_TARGET} PRIVATE ${IOKIT_LIB})
        find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
        target_link_libraries(${YOUR_TARGET} PRIVATE ${CORE_FOUNDATION_FRAMEWORK})
        find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
        target_link_libraries(${YOUR_TARGET} PRIVATE ${CORE_VIDEO_FRAMEWORK})
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        find_package(Threads)
        find_package(X11)
        target_link_libraries(${YOUR_TARGET} PRIVATE ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xinerama Xcursor Xi m dl ${X11_LIBRARIES})
    endif()

endmacro()
