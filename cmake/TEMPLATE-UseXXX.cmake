# Template for creating a UseSOMETHING.cmake file for finding or downloading dependencies.
# Copy this, replace XXX with the name of the dependency, and add any additional tweaks
# that are needed (e.g., adding XXX's dependencies, setting OS-specific compile flags) in 
# order to build ${YOUR_TARGET} with libXXX.
# -----------------------------------------------------------------------------------------

# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Tries to find a pre-installed version of XXX on this system using find_package().
# Case 1: If found, then the script uses target_link_library() to link it to your target.
# Case 2: If not found and AUTOBUILD_DEPENDENCIES is ON, then the script immediately 
# downloads, builds, and installs the XXX library to the location specifed by
# CMAKE_INSTALL_PREFIX. Then, it tries find_package() again and links to the newly install
# library.
# Case 3: If not found and AUTOBUILD_DEPENDENCIES is OFF, then the script exits with a
# fatal error.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the XXX library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseXXX)
#    UseXXX(${PROJECT_NAME} PRIVATE)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html



macro(UseXXX YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for XXX library...")

    # Check to see if the library is already installed on the system

    # XXX Add a note here on what you are hoping to find, typically:
    # A: CMake might ship with a FindXXX.cmake module that defines an imported target like XXX::XXX
    # B: XXX might install a cmake config package (e.g., /lib/cmake/XXX/XXXConfig.cmake) and it might
    #    define an imported target like XXX::XXX.  In this case use find_package(XXX CONFIG).
    # C: If all else fails, we can write our own cmake/Modules/FindXXX.cmake and use it.
    find_package(XXX)

    # Case 1: Already installed on the system
    if (${XXX_FOUND})

        message(STATUS "Ok: XXX Found.")
        message(STATUS "XXX headers: ${XXX_INCLUDE_DIR}")
        message(STATUS "XXX libs: ${XXX_LIBRARIES}")
    

    # Case 2: Download, build and install it now for the user, then try find_package() again
    elseif (AUTOBUILD_DEPENDENCIES)

        set(XXX_AUTOBUILT TRUE)

        message(STATUS "Ok: AUTOBUILD_DEPENDENCIES is ON so XXX will be downloaded into the external directory and built for you.")

        include(MinVRExternalProject)
        MinVRExternalProject_Download(
            XXX
            URL https://sourceforge.net/projects/XXX/files/XXX/2.1.0/XXX-2.1.0.zip
        )

        MinVRExternalProject_BuildAndInstallNow(
            XXX
            src
        )

        # Try find_package() again
        message(STATUS "Searching (again, right after autobuilding) for XXX library...")
        find_package(XXX)
 
        # We just intalled it to CMAKE_INSTALL_PREFIX, and the root CMakeLists.txt puts this in the
        # CMAKE_MODULE_PATH.  So, if we were not able to find the package now, then something is very wrong.
        if (NOT ${XXX_FOUND})
            message(FATAL_ERROR "MinVR did an autobuild of the XXX dependency, and it should now be installed at the prefix ${CMAKE_INSATALL_PREFIX}, but cmake is still unable to find it with find_package().")
        endif()

        message(STATUS "XXX headers: ${XXX_INCLUDE_DIR}")
        message(STATUS "XXX libs: ${XXX_LIBRARIES}")


    # Case 3: The user does not want us to build it, so error out when not found.
    else()

        message(FATAL_ERROR "The XXX library was not found on the system.  You can: (1) install XXX yourself, (2)point cmake to an already-installed version of XXX by adding the installation prefix of XXX to the CMAKE_PREFIX_PATH environment variable, or (3) set AUTOBUILD_DEPENDENCIES to ON to have MinVR download, build, and install XXX for you.")

    endif()


    # If we reach this point without an error, then one of the calls to find_package() was successful

    message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency XXX::XXX.")

    if (TARGET XXX::XXX)
        # If find_package() created an imported target, then link with it.  This will automatically add include
        # dirs and compile flags associated with the target to our build.  This is the ideal situation.
        target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} XXX::XXX)
    else()
        # Older cmake modules or incomplete config packages may not create an imported target.  In thise case
        # use the old-style cmake approach of manually adding the libraries and includes based on the variables
        # defined in FindXXX.cmake.  Be careful -- some Find scripts use XXX_INCLUDE_DIRS (plural) and some use
        # XXX_INCLUDE_DIR. 
        target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${XXX_LIBRARIES})
        target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${XXX_INCLUDE_DIR})
    endif()
    
    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_XXX)

endmacro()

