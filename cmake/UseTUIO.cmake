# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Either finds a pre-installed version or downloads and builds the TUIO library.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the TUIO library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseTUIO)
#    UseTUIO(${PROJECT_NAME} PUBLIC)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html

# Effect: This will use find_package() to try to find a version of TUIO already on 
# the system.  If found, that pre-installed version will be used to build the target.
# If no pre-installed version is found, then a new target will be added to the current
# build in order to download and build TUIO as an external project.  In this case, the
# same flags that find_package() sets (TUIO_INCLUDE_DIR, etc.) will be set to point to
# the newly build version so the rest of your CMakeLists.txt files can treat this as if
# the find_package() step were successful.  In either case, TUIO will be linked to your
# project using target_include_directories() and target_link_libraries().  Finally, 
# if you eventually install your project, then cmake will also install the newly built
# version of TUIO to the same install prefix as for your project.

macro(UseTUIO YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for TUIO library...")

    # Check to see if the library is already installed on the system
    # CMake does not provide a FindTUIO.cmake, and TUIO does not provide a config package.
    # So, the call below is using our own cmake/FindTUIO.cmake script.
    find_package(TUIO)

    if (${TUIO_FOUND})
        message(STATUS "Ok: TUIO Found.")

        target_link_libraries(${YOUR_TARGET} ${TUIO_LIBRARIES})
        target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${TUIO_INCLUDE_DIR})
    else()

        # Either autobuild it or provide an informative error message
        if (DEPENDENCIES_AUTOBUILD)

            set(TUIO_AUTOBUILT TRUE)

            message(STATUS "Ok: AUTOBUILD_PLUGIN_DEPENDENCIES is ON so TUIO will be downloaded into the external directory and built for you.")

            if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
                set(TUIO_CMAKE_ARGS -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD=c++11 -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY=libc++)
            endif()

            include(ExternalProject)
            ExternalProject_add(TUIO
                PREFIX ${EXTERNAL_DIR}/TUIO
                URL http://prdownloads.sourceforge.net/reactivision/TUIO_CPP-1.4.zip
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/TUIO/install ${TUIO_CMAKE_ARGS}
                PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/external-patches/TUIO/CMakeLists.txt <SOURCE_DIR> && ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/external-patches/TUIO/TuioServer.cpp <SOURCE_DIR>/TUIO
            )
            add_dependencies(${YOUR_TARGET} TUIO)


            # Set the same variables that find_package would
            set(TUIO_INCLUDE_DIR "${EXTERNAL_DIR}/TUIO/install/include")
            set(TUIO_LIBRARY_DIR "${EXTERNAL_DIR}/TUIO/install/lib")
            if(MSVC)
                set(TUIO_LIBRARIES optimized ${TUIO_LIBRARY_DIR}/TUIO.lib debug ${TUIO_LIBRARY_DIR}/TUIOd.lib)
                set(TUIO_LIBRARIES ${TUIO_LIBRARIES} winmm.lib wininet.lib ws2_32.lib)
            else()
                set(TUIO_LIBRARIES optimized ${TUIO_LIBRARY_DIR}/libTUIO.a debug ${TUIO_LIBRARY_DIR}/libTUIOd.a)
            endif()


            # When we install MinVR, install the new library to the same place so other projects
            # including future clean builds of MinVR can use it.  Note, install step is separate
            # from the one that is optionally included in ExternalProject_add.  If you setup the
            # external project to install, that install refers to whatever install system is
            # builtin to the external project's build system, whereas this install refers to the
            # MinVR library installation process.
            if (DEPENDENCIES_INSTALL)
                install(DIRECTORY ${TUIO_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/TUIO)
                install(DIRECTORY ${TUIO_LIBRARY_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
            endif()

            target_link_libraries(${YOUR_TARGET} ${TUIO_LIBRARIES})
            target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} 
                $<BUILD_INTERFACE:${TUIO_INCLUDE_DIR}> # for headers when building
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/TUIO>  # for client in install mode
            )

        else()

            message(FATAL_ERROR "The TUIO library was not found on the system.  You can: (1) install TUIO yourself, (2)point cmake to an already-installed version of TUIO by adding the installation prefix of TUIO to the CMAKE_PREFIX_PATH variable, or (3) set DEPENDENCIES_AUTOBUILD to ON to have MinVR download and build TUIO in its 'external' directory for you.")

        endif()

    endif()

    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_TUIO)

endmacro()
