# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Either finds a pre-installed version or downloads and builds the FreeGLUT library.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the FreeGLUT library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseFreeGLUT)
#    UseFreeGLUT(${PROJECT_NAME} PUBLIC)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html

# Effect: This will use find_package() to try to find a version of FreeGLUT already on 
# the system.  If found, that pre-installed version will be used to build the target.
# If no pre-installed version is found, then a new target will be added to the current
# build in order to download and build FreeGLUT as an external project.  In this case, the
# same flags that find_package() sets (FREEGLUT_INCLUDE_DIR, etc.) will be set to point to
# the newly build version so the rest of your CMakeLists.txt files can treat this as if
# the find_package() step were successful.  In either case, FreeGLUT will be linked to your
# project using target_include_directories() and target_link_libraries().  Finally, 
# if you eventually install your project, then cmake will also install the newly built
# version of FreeGLUT to the same install prefix as for your project.

# NOTE November 2017: FreeGLUT seems to not build on OSX 10.10 at the moment -- looking
# at the forums, this is a known bug in FreeGLUT, not a problem with this build system.


macro(UseFreeGLUT YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for FreeGLUT library...")

    # Check to see if the library is already installed on the system
    # CMake provides FindGLUT, which finds either glut and freeglut, but MinVR only
    # works with freeglut, so the call below is using our own config/FindFreeGLUT.cmake
    # script.
    find_package(FreeGLUT)


    if (${FREEGLUT_FOUND})
        message(STATUS "Ok: FreeGLUT Found.")

        target_link_libraries(${YOUR_TARGET} ${FREEGLUT_LIBRARIES})
        target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${FREEGLUT_INCLUDE_DIR})
    else()

        # Either autobuild it or provide an informative error message
        if (DEPENDENCIES_AUTOBUILD)

            set(FREEGLUT_AUTOBUILT TRUE)

            message(STATUS "Ok: AUTOBUILD_PLUGIN_DEPENDENCIES is ON so FreeGLUT will be downloaded into the external directory and built for you.")

            include(ExternalProject)
            ExternalProject_add(FreeGLUT
                PREFIX ${EXTERNAL_DIR}/FreeGLUT
                SVN_REPOSITORY https://svn.code.sf.net/p/freeglut/code/trunk/freeglut/freeglut
                SVN_TRUST_CERT 1
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/FreeGLUT/install ${FREEGLUT_CMAKE_ARGS}
            )
            add_dependencies(${YOUR_TARGET} FreeGLUT)


            # Set the same variables that find_package would
            set(FREEGLUT_INCLUDE_DIR "${EXTERNAL_DIR}/FreeGLUT/install/include")
            set(FREEGLUT_LIBRARY_DIR "${EXTERNAL_DIR}/FreeGLUT/install/lib")
            if (MSVC)
                set(FREEGLUT_LIBRARIES "${FREEGLUT_LIBRARY_DIR}/freeglut.lib")
            else()
                set(FREEGLUT_LIBRARIES "${FREEGLUT_LIBRARY_DIR}/freeglut.a")
            endif()

            # When we install MinVR, install the new library to the same place so other projects
            # including future clean builds of MinVR can use it.  Note, install step is separate
            # from the one that is optionally included in ExternalProject_add.  If you setup the
            # external project to install, that install refers to whatever install system is
            # builtin to the external project's build system, whereas this install refers to the
            # MinVR library installation process.
            if (DEPENDENCIES_INSTALL)
                install(DIRECTORY ${FREEGLUT_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
                install(DIRECTORY ${FREEGLUT_LIBRARY_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
            endif()

            target_link_libraries(${YOUR_TARGET} ${FREEGLUT_LIBRARIES})
            target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} 
                $<BUILD_INTERFACE:${FREEGLUT_INCLUDE_DIR}> # for headers when building
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>  # for client in install mode
            )

        else()

            message(FATAL_ERROR "The FreeGLUT library was not found on the system.  You can: (1) install FreeGLUT yourself, (2)point cmake to an already-installed version of FreeGLUT by adding the installation prefix of FreeGLUT to the CMAKE_PREFIX_PATH variable, or (3) set DEPENDENCIES_AUTOBUILD to ON to have MinVR download and build FreeGLUT in its 'external' directory for you.")

        endif()

    endif()

    message(STATUS "Adding build flags for FreeGLUT programs.")

    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_FREEGLUT)

endmacro()
