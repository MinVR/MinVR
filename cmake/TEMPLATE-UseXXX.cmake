# Template for creating a UseSOMETHING.cmake file for finding or downloading dependencies.
# Copy this, replace XXX with the name of the dependency, and add any additional tweaks
# that are needed (e.g., adding XXX's dependencies, setting OS-specific compile flags) in 
# order to build ${YOUR_TARGET} with libXXX.
# -----------------------------------------------------------------------------------------

# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Either finds a pre-installed version or downloads and builds the XXX library.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the XXX library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseXXX)
#    UseXXX(${PROJECT_NAME} PUBLIC)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html

# Effect: This will use find_package() to try to find a version of XXX already on 
# the system.  If found, that pre-installed version will be used to build the target.
# If no pre-installed version is found, then a new target will be added to the current
# build in order to download and build XXX as an external project.  In this case, the
# same flags that find_package() sets (XXX_INCLUDE_DIR, etc.) will be set to point to
# the newly build version so the rest of your CMakeLists.txt files can treat this as if
# the find_package() step were successful.  In either case, XXX will be linked to your
# project using target_include_directories() and target_link_libraries().  Finally, 
# if you eventually install your project, then cmake will also install the newly built
# version of XXX to the same install prefix as for your project.


macro(UseXXX)

    message(STATUS "Searching for XXX library...")

    # Check to see if the library is already installed on the system

    # Note to developers:  For this step, we try to use CMake's FindXXX.cmake if it 
    # comes with one.  Or XXX's XXXConfig.cmake if it was installed with one.  If XXX is
    # not supported via either of these options, then the fallback is to write your own
    # FindXXX.cmake and put it in MinVR's cmake/Modules/ directory.
    find_package(XXX)

    if (${XXX_FOUND})
        message(STATUS "Ok: XXX Found.")

        target_link_libraries(${YOUR_TARGET} ${XXX_LIBRARIES})
        target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${XXX_INCLUDE_DIR})
    else()

        # Either autobuild it or provide an informative error message
        if (DEPENDENCIES_AUTOBUILD)

            set(XXX_AUTOBUILT TRUE)

            message(STATUS "Ok: AUTOBUILD_PLUGIN_DEPENDENCIES is ON so XXX will be downloaded into the external directory and built for you.")


            # Note to developers:  See CMake docs for full list of options for ExternalProject_add
            #     https://cmake.org/cmake/help/v3.9/module/ExternalProject.html
            # Some key points are:
            # 1. If you set PREFIX, then the source, install, tmp, and stamp directories all default to reasonable values
            # 2. If the project supports an install step and you want this to be called during the build process it is not
            #    enough to just set the INSTALL_DIR variable, you must pass the -DCMAKE_INSTALL_PREFIX=... flag through to
            #    CMAKE_ARGS in order to trigger the install.
            # 3. You can download projects from GIT, SVN, CVS, etc. or zip files from URL's, so this can support almost any
            #    type of online project.

            include(ExternalProject)
            ExternalProject_add(XXX
                PREFIX ${EXTERNAL_DIR}/XXX
                GIT_REPOSITORY https://github.com/XXX
                GIT_TAG master
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/XXX/install ${XXX_CMAKE_ARGS}
            )
            add_dependencies(${YOUR_TARGET} XXX)

            # Set the same variables that find_package would
            set(XXX_INCLUDE_DIR "${EXTERNAL_DIR}/XXX/install/include")
            set(XXX_LIBRARY_DIR "${EXTERNAL_DIR}/XXX/install/lib")
            if(MSVC)
                set(XXX_LIBRARIES optimized ${XXX_LIBRARY_DIR}/XXX.lib debug ${XXX_LIBRARY_DIR}/XXXd.lib)
            else()
                set(XXX_LIBRARIES optimized ${XXX_LIBRARY_DIR}/libXXX.a debug ${XXX_LIBRARY_DIR}/libXXXd.a)
            endif()


            # When we install MinVR, install the new library to the same place so other projects
            # including future clean builds of MinVR can use it.  Note, install step is separate
            # from the one that is optionally included in ExternalProject_add.  If you setup the
            # external project to install, that install refers to whatever install system is
            # builtin to the external project's build system, whereas this install refers to the
            # MinVR library installation process.
            if (DEPENDENCIES_INSTALL)
                install(DIRECTORY ${XXX_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/XXX)
                install(DIRECTORY ${XXX_LIBRARY_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
            endif()

            target_link_libraries(${YOUR_TARGET} ${XXX_LIBRARIES})
            target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} 
                $<BUILD_INTERFACE:${XXX_INCLUDE_DIRS}> # for headers when building
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/XXX>  # for client in install mode
            )

        else()

            message(FATAL_ERROR "The XXX library was not found on the system.  You can: (1) install XXX yourself, (2)point cmake to an already-installed version of XXX by adding the installation prefix of XXX to the CMAKE_PREFIX_PATH variable, or (3) set AUTOBUILD_DEPENDENCIES to ON to have MinVR download and build XXX in its 'external' directory for you.")

        endif()

    endif()

    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_XXX)

endmacro()
