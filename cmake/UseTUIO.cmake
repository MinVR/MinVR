# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Tries to find a pre-installed version of TUIO on this system using find_package().
# Case 1: If found, then the script uses target_link_library() to link it to your target.
# Case 2: If not found and AUTOBUILD_DEPENDENCIES is ON, then the script immediately 
# downloads, builds, and installs the TUIO library to the location specifed by
# CMAKE_INSTALL_PREFIX. Then, it tries find_package() again and links to the newly install
# library.
# Case 3: If not found and AUTOBUILD_DEPENDENCIES is OFF, then the script exits with a
# fatal error.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the TUIO library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseTUIO)
#    UseTUIO(${PROJECT_NAME} PRIVATE)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html



macro(UseTUIO YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for TUIO library...")

    # Check to see if the library is already installed on the system
    # CMake does not provide a FindTUIO.cmake, and TUIO does not provide a config package.
    # So, the call below is using our own cmake/Modules/FindTUIO.cmake script.
    find_package(TUIO)

    # Case 1: Already installed on the system
    if (${TUIO_FOUND})

        message(STATUS "Ok: TUIO Found.")


    # Case 2: Download, build and install it now for the user, then try find_package() again
    elseif (AUTOBUILD_DEPENDENCIES)

        set(TUIO_AUTOBUILT TRUE)

        message(STATUS "Ok: AUTOBUILD_DEPENDENCIES is ON so TUIO will be downloaded into the external directory and built for you.")

        include(MinVRExternalProject)
        MinVRExternalProject_Download(
            TUIO
            URL http://prdownloads.sourceforge.net/reactivision/TUIO_CPP-1.4.zip
            PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${MINVR_PROJECT_ROOT}/external-patches/TUIO/CMakeLists.txt <SOURCE_DIR> && ${CMAKE_COMMAND} -E copy ${MINVR_PROJECT_ROOT}/external-patches/TUIO/TuioServer.cpp <SOURCE_DIR>/TUIO
        )

        if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(TUIO_CONFIG_OPTIONS
                -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD=c++11 
                -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY=libc++
            )
        endif()

        MinVRExternalProject_BuildAndInstallNow(
            TUIO
            src
            ${TUIO_CONFIG_OPTIONS}
        )

        # Try find_package() again
        message(STATUS "Searching (again, right after autobuilding) for TUIO library...")
        find_package(TUIO)

        message(STATUS ${CMAKE_PREFIX_PATH})


        # We just intalled it to CMAKE_INSTALL_PREFIX, and the root CMakeLists.txt puts this in the
        # CMAKE_MODULE_PATH.  So, if we were not able to find the package now, then something is very wrong.
        if (NOT ${TUIO_FOUND})
            message(FATAL_ERROR "MinVR did an autobuild of the TUIO dependency, and it should now be installed at the prefix ${CMAKE_INSTALL_PREFIX}, but cmake is still unable to find it with find_package().")
        endif()


    # Case 3: The user does not want us to build it, so error out when not found.
    else()

        message(FATAL_ERROR "The TUIO library was not found on the system.  You can: (1) install TUIO yourself, (2)point cmake to an already-installed version of TUIO by adding the installation prefix of TUIO to the CMAKE_PREFIX_PATH environment variable, or (3) set AUTOBUILD_DEPENDENCIES to ON to have MinVR download, build, and install TUIO for you.")

    endif()


    # If we reach this point without an error, then one of the calls to find_package() was successful

    message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency TUIO.")

    # This uses "old-style" cmake, rather than importing a target from a cmake config file.
    # We use FindTUIO to set TUIO_LIBRARIES AND TUIO_INCLUDE_DIR and then we add 
    # these manually to our target.

    target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${TUIO_LIBRARIES})
    target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${TUIO_INCLUDE_DIR})
    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_TUIO)

    message(STATUS "Adding TUIO's OS-specific build flags and dependencies to target ${YOUR_TARGET}.")
    if (MSVC)
        target_link_libraries(${PROJECT_NAME} ${INTERFACE_PUBLIC_OR_PRIVATE} winmm.lib wininet.lib ws2_32.lib)
    endif()

endmacro()
