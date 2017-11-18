# Template for creating a UseSOMETHING.cmake file for finding or downloading dependencies.
# Copy this, and replace XXX with the name of the dependency.
# -----------------------------------------------------------------------------------------

# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Either finds a pre-installed version or downloads and builds the XXX library.
#
# Usage: In your CMakeLists.txt:
# 1. Somewhere before your add_executable() or add_library() line, add the following:
#        include(UseXXX)
#        UseXXX()
# 2. After your add_executable() or add_library() line, add the following:
#        if (${XXX_AUTOBUILT})
#            add_dependencies(${PROJECT_NAME} XXX)
#        endif()
#
# Effect: This will use find_package() to try to find a version of XXX already on 
# the system.  If found, that pre-installed version will be used to build the target.
# If no pre-installed version is found, then a new target will be added to the current
# build in order to download and build XXX as an external project.  In this case, the
# same flags that find_package() sets (XXX_INCLUDE_DIR, etc.) will be set to point to
# the newly build version so the rest of your CMakeLists.txt files can treat this as if
# the find_package() step were successful.  Finally, if you eventually install your own
# project, then cmake will also install the newly built version of XXX to the same
# install prefix as for your project.


macro(UseXXX)

    message(STATUS "Searching for XXX library...")

    # Check to see if the library is already installed on the system
    find_package(XXX)

    if (${XXX_FOUND})
        message(STATUS "Ok: XXX Found.")
    else()

        # Either autobuild it or provide an informative error message
        if (DEPENDENCIES_AUTOBUILD)

            set(XXX_AUTOBUILT TRUE)

            message(STATUS "Ok: AUTOBUILD_PLUGIN_DEPENDENCIES is ON so XXX will be downloaded into the external directory and built for you.")


            # See CMake docs for full list of options for ExternalProject_add
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

            # Set the same variables that find_package would
            set(XXX_INCLUDE_DIR "${EXTERNAL_DIR}/XXX/install/include")
            set(XXX_LIBRARY_DIR "${EXTERNAL_DIR}/XXX/install/lib")
            if(MSVC)
                set(XXX_LIBRARIES optimized XXX.lib debug XXXd.lib)
            else()
                set(XXX_LIBRARIES optimized libXXX.a debug libXXXd.a)
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

        else()

            message(FATAL_ERROR "The XXX library was not found on the system.  You can: (1) install XXX yourself, (2)point cmake to an already-installed version of XXX by adding the installation prefix of XXX to the CMAKE_PREFIX_PATH variable, or (3) set AUTOBUILD_DEPENDENCIES to ON to have MinVR download and build XXX in its 'external' directory for you.")

        endif()

    endif()

    message(STATUS "Adding build flags for XXX programs.")

    add_definitions(-DUSE_XXX)

    if (MSVC)
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    endif()

endmacro()
