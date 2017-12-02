# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Either finds a pre-installed version or downloads and builds the GLFW library.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the GLFW library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseGLFW)
#    UseGLFW(${PROJECT_NAME} PUBLIC)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html

# Effect: This will use find_package() to try to find a version of GLFW already on 
# the system.  If found, that pre-installed version will be used to build the target.
# If no pre-installed version is found, then a new target will be added to the current
# build in order to download and build GLFW as an external project.  In this case, the
# same flags that find_package() sets (GLFW_INCLUDE_DIR, etc.) will be set to point to
# the newly build version so the rest of your CMakeLists.txt files can treat this as if
# the find_package() step were successful.  In either case, GLFW will be linked to your
# project using target_include_directories() and target_link_libraries().  Finally, 
# if you eventually install your project, then cmake will also install the newly built
# version of GLFW to the same install prefix as for your project.


macro(UseGLFW YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for GLFW library...")

    # Check to see if the library is already installed on the system
    # - CMake does not provide FindGLFW.
    # - GLFW does provide a cmake config package GLFWConfig.cmake, but it is not written
    #   correctly -- it uses hardcoded paths and is not relocatable.
    # - So, the call below is using our own cmake/FindGLFW.cmake routine
    # - We will likely want to update this in the future as GLFW and cmake provide more support.
    find_package(GLFW)

    if (${GLFW_FOUND})
        message(STATUS "Ok: Found GLFW.  Linking to ${YOUR_TARGET} target.")

        target_link_libraries(${YOUR_TARGET} ${GLFW_LIBRARIES})
        target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLFW_INCLUDE_DIR})

    else()

        # Either autobuild it or provide an informative error message
        if (DEPENDENCIES_AUTOBUILD)

            set(GLFW_AUTOBUILT TRUE)

            message(STATUS "Ok: AUTOBUILD_PLUGIN_DEPENDENCIES is ON so GLFW will be downloaded into the external directory and built for you.")
             
            include(ExternalProject)
            ExternalProject_add(GLFW
                PREFIX ${EXTERNAL_DIR}/GLFW
                GIT_REPOSITORY https://github.com/ivlab/glfw.git
                GIT_TAG gpu-affinity
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/GLFW/install ${GLFW_CMAKE_ARGS}
            )
            add_dependencies(${YOUR_TARGET} GLFW)


            # Set the same variables that find_package would
            set(GLFW_INCLUDE_DIR "${EXTERNAL_DIR}/GLFW/install/include")
            set(GLFW_LIBRARY_DIR "${EXTERNAL_DIR}/GLFW/install/lib")
            if (MSVC)
                set(GLFW_LIBRARIES "${EXTERNAL_DIR}/GLFW/install/lib/glfw3.lib")
            else()
                set(GLFW_LIBRARIES "${EXTERNAL_DIR}/GLFW/install/lib/libglfw3.a")
            endif()

            # When we install MinVR, install the new library to the same place so other projects
            # including future clean builds of MinVR can use it.  Note, install step is separate
            # from the one that is optionally included in ExternalProject_add.  If you setup the
            # external project to install, that install refers to whatever install system is
            # builtin to the external project's build system, whereas this install refers to the
            # MinVR library installation process.
            if (DEPENDENCIES_INSTALL)
                install(DIRECTORY ${GLFW_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
                # excluding cmake directory b/c GLFWConfig.cmake is not relocatable
                install(DIRECTORY ${GLFW_LIBRARY_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib PATTERN "cmake" EXCLUDE)
            endif()

            target_link_libraries(${YOUR_TARGET} ${GLFW_LIBRARIES})
            target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} 
                $<BUILD_INTERFACE:${GLFW_INCLUDE_DIR}> # for headers when building
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>  # for client in install mode
            )

        else()

            message(FATAL_ERROR "The GLFW library was not found on the system.  You can: (1) install GLFW yourself, (2)point cmake to an already-installed version of GLFW by adding the installation prefix of GLFW to the CMAKE_PREFIX_PATH variable, or (3) set DEPENDENCIES_AUTOBUILD to ON to have MinVR download and build GLFW in its 'external' directory for you.")

        endif()

    endif()

    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_GLFW)

    message(STATUS "Adding GLFW's OS-specific build flags and dependencies to target ${YOUR_TARGET}.")

    if (MSVC)
        target_compile_definitions(${YOUR_TARGET} PUBLIC -D_CRT_SECURE_NO_WARNINGS)
        # Tell MSVC to use main instead of WinMain for Windows subsystem executables
        set_target_properties(${WINDOWS_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
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
