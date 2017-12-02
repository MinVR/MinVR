# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Either finds a pre-installed version or downloads and builds the OpenVR library.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the OpenVR library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseOpenVR)
#    UseOpenVR(${PROJECT_NAME} PUBLIC)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html

# Effect: This will use find_package() to try to find a version of OpenVR already on 
# the system.  If found, that pre-installed version will be used to build the target.
# If no pre-installed version is found, then a new target will be added to the current
# build in order to download and build OpenVR as an external project.  In this case, the
# same flags that find_package() sets (OPENVR_INCLUDE_DIR, etc.) will be set to point to
# the newly build version so the rest of your CMakeLists.txt files can treat this as if
# the find_package() step were successful.  In either case, OpenVR will be linked to your
# project using target_include_directories() and target_link_libraries().  Finally, 
# if you eventually install your project, then cmake will also install the newly built
# version of OpenVR to the same install prefix as for your project.

macro(UseOpenVR YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for OpenVR library...")

    # Check to see if the library is already installed on the system
    # CMake does not provide a FindOpenVR.cmake, and OpenVR does not provide a config package.
    # So, the call below is using our own cmake/FindOpenVR.cmake script.
    find_package(OpenVR)

    if (${OPENVR_FOUND})
        message(STATUS "Ok: OpenVR Found.")

        target_link_libraries(${YOUR_TARGET} ${OPENVR_LIBRARIES})
        target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${OPENVR_INCLUDE_DIR})
    else()

        # Either autobuild it or provide an informative error message
        if (DEPENDENCIES_AUTOBUILD)

            set(OPENVR_AUTOBUILT TRUE)

            message(STATUS "Ok: AUTOBUILD_PLUGIN_DEPENDENCIES is ON so OpenVR will be downloaded into the external directory and built for you.")

            include(ExternalProject)
            ExternalProject_add(OpenVR
                PREFIX ${EXTERNAL_DIR}/OpenVR
                GIT_REPOSITORY https://github.com/ivlab/openvr
                GIT_TAG master
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/OpenVR/install -DCMAKE_C_FLAGS=-fPIC -DQNANHIBIT=1 ${OPENVR_CMAKE_ARGS}
            )
            add_dependencies(${YOUR_TARGET} OpenVR)


            # Set the same variables that find_package would
            set(OPENVR_INCLUDE_DIR "${EXTERNAL_DIR}/OpenVR/src/OpenVR/headers")
            set(OPENVR_LIBRARY_DIR "${EXTERNAL_DIR}/OpenVR/install/lib")
            if (MSVC)
                set(OPENVR_LIBRARIES "${OPENVR_LIBRARY_DIR}/openvr_api.lib")
            else()
                set(OPENVR_LIBRARIES "${OPENVR_LIBRARY_DIR}/libopenvr_api.a")
            endif()

            # When we install MinVR, install the new library to the same place so other projects
            # including future clean builds of MinVR can use it.  Note, install step is separate
            # from the one that is optionally included in ExternalProject_add.  If you setup the
            # external project to install, that install refers to whatever install system is
            # builtin to the external project's build system, whereas this install refers to the
            # MinVR library installation process.
            if (DEPENDENCIES_INSTALL)
                install(DIRECTORY ${OPENVR_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/openvr)
                install(DIRECTORY ${OPENVR_LIBRARY_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
            endif()

            target_link_libraries(${YOUR_TARGET} ${OPENVR_LIBRARIES})
            target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} 
                $<BUILD_INTERFACE:${OPENVR_INCLUDE_DIR}> # for headers when building
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/openvr>  # for client in install mode
            )

        else()

            message(FATAL_ERROR "The OpenVR library was not found on the system.  You can: (1) install OpenVR yourself, (2)point cmake to an already-installed version of OpenVR by adding the installation prefix of OpenVR to the CMAKE_PREFIX_PATH variable, or (3) set DEPENDENCIES_AUTOBUILD to ON to have MinVR download and build OpenVR in its 'external' directory for you.")

        endif()

    endif()

    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_OPENVR)

    message(STATUS "Adding GLFW's OS-specific build flags and dependencies to target ${YOUR_TARGET}.")


    if (MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        # Tell MSVC to use main instead of WinMain for Windows subsystem executables
        set_target_properties(${WINDOWS_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
#        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        find_library(COCOA_LIB Cocoa)
        target_link_libraries(${YOUR_TARGET} ${COCOA_LIB})
        find_library(IOKIT_LIB IOKit)
        target_link_libraries(${YOUR_TARGET} ${IOKIT_LIB})
        find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
        target_link_libraries(${YOUR_TARGET} ${CORE_FOUNDATION_FRAMEWORK})
        find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
        target_link_libraries(${YOUR_TARGET} ${CORE_VIDEO_FRAMEWORK})
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        find_package(Threads)
        find_package(X11)
        target_link_libraries(${YOUR_TARGET} ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xinerama Xcursor Xi m dl ${X11_LIBRARIES})
    endif()

endmacro()
