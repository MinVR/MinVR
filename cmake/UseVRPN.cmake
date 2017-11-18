# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Either finds a pre-installed version or downloads and builds the VRPN library.
#
# Usage: In your CMakeLists.txt:
# 1. Somewhere before your add_executable() or add_library() line, add the following:
#        include(UseVRPN)
#        UseVRPN()
# 2. After your add_executable() or add_library() line, add the following:
#        if (${VRPN_AUTOBUILT})
#            add_dependencies(${PROJECT_NAME} VRPN)
#        endif()
#
# Effect: This will use find_package() to try to find a version of VRPN already on 
# the system.  If found, that pre-installed version will be used to build the target.
# If no pre-installed version is found, then a new target will be added to the current
# build in order to download and build VRPN as an external project.  In this case, the
# same flags that find_package() sets (VRPN_INCLUDE_DIR, etc.) will be set to point to
# the newly build version so the rest of your CMakeLists.txt files can treat this as if
# the find_package() step were successful.  Finally, if you eventually install your own
# project, then cmake will also install the newly built version of VRPN to the same
# install prefix as for your project.


macro(UseVRPN)

    message(STATUS "Searching for VRPN library...")

    # Check to see if the library is already installed on the system
    find_package(VRPN)

    if (${VRPN_FOUND})
        message(STATUS "Ok: VRPN Found.")
    else()

        # Either autobuild it or provide an informative error message
        if (DEPENDENCIES_AUTOBUILD)

            set(VRPN_AUTOBUILT TRUE)

            message(STATUS "Ok: AUTOBUILD_PLUGIN_DEPENDENCIES is ON so VRPN will be downloaded into the external directory and built for you.")

            set(VRPN_CMAKE_ARGS -DVRPN_BUILD_DIRECTSHOW_VIDEO_SERVER=OFF -DVRPN_BUILD_HID_GUI=OFF -DVRPN_BUILD_JAVA=OFF -DVRPN_BUILD_PYTHON=OFF -DVRPN_USE_DIRECTINPUT=OFF -DVRPN_USE_DIRECTSHOW=OFF -DCMAKE_BUILD_TYPE=Release -Wno-dev)

            if(MSVC)
                set(VRPN_CMAKE_ARGS ${VRPN_CMAKE_ARGS} -DVRPN_USE_HID=ON -DVRPN_HID_DEBUGGING=ON -DVRPN_USE_LOCAL_HIDAPI=ON)
            endif()

            include(ExternalProject)
            ExternalProject_add(VRPN
                PREFIX ${EXTERNAL_DIR}/VRPN
                URL https://github.com/vrpn/vrpn/releases/download/v07.33/vrpn_07_33.zip
                INSTALL_DIR ${EXTERNAL_DIR}/VRPN/install
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/VRPN/install -DVRPN_INSTALL=TRUE ${VRPN_CMAKE_ARGS}
            )

            # Set the same variables that find_package would

            set(VRPN_INCLUDE_DIR ${EXTERNAL_DIR}/VRPN/install/include)
            set(VRPN_LIBRARY_DIR "${EXTERNAL_DIR}/VRPN/install/lib")
            set(VRPN_BIN_DIR "${EXTERNAL_DIR}/VRPN/install/bin")

            
            if (MSVC)
                set(VRPN_LIBRARIES optimized vrpn.lib optimized quat.lib debug vrpnd.lib debug quatd.lib)
            else()
                set(VRPN_LIBRARIES libvrpn.a libquat.a)
            endif() 


            # When we install MinVR, install the new library to the same place so other projects
            # including future clean builds of MinVR can use it.  Note, install step is separate
            # from the one that is optionally included in ExternalProject_add.  If you setup the
            # external project to install, that install refers to whatever install system is
            # builtin to the external project's build system, whereas this install refers to the
            # MinVR library installation process.
            if (DEPENDENCIES_INSTALL)
                install(DIRECTORY ${VRPN_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/vrpn)
                install(DIRECTORY ${VRPN_LIBRARY_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
                install(DIRECTORY ${VRPN_BIN_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
            endif()

        else()

            message(FATAL_ERROR "The VRPN library was not found on the system.  You can: (1) install VRPN yourself, (2)point cmake to an already-installed version of VRPN by adding the installation prefix of VRPN to the CMAKE_PREFIX_PATH variable, or (3) set DEPENDENCIES_AUTOBUILD to ON to have MinVR download and build VRPN in its 'external' directory for you.")

        endif()

    endif()

    message(STATUS "Adding build flags for VRPN programs.")

    add_definitions(-DUSE_VRPN)

    if (MSVC)
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    endif()

endmacro()
