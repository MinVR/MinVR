# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Either finds a pre-installed version or downloads and builds the GLFW library.
#
# Usage: In your CMakeLists.txt:
# 1. Somewhere before your add_executable() or add_library() line, add the following:
#        include(UseGLFW)
#        UseGLFW()
# 2. After your add_executable() or add_library() line, add the following:
#        if (${GLFW_AUTOBUILT})
#            add_dependencies(${PROJECT_NAME} GLFW)
#        endif()
#
# Effect: This will use find_package() to try to find a version of GLFW already on 
# the system.  If found, that pre-installed version will be used to build the target.
# If no pre-installed version is found, then a new target will be added to the current
# build in order to download and build GLFW as an external project.  In this case, the
# same flags that find_package() sets (GLFW_INCLUDE_DIR, etc.) will be set to point to
# the newly build version so the rest of your CMakeLists.txt files can treat this as if
# the find_package() step were successful.  Finally, if you eventually install your own
# project, then cmake will also install the newly built version of GLFW to the same
# install prefix as for your project.


macro(UseGLFW)

    message(STATUS "Searching for GLFW library...")

    # Check to see if the library is already installed on the system
    find_package(GLFW)

    if (${GLFW_FOUND})
        message(STATUS "Ok: GLFW Found.")
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

            # Set the same variables that find_package would
            set(GLFW_INCLUDE_DIR "${EXTERNAL_DIR}/GLFW/install/include")
            set(GLFW_LIBRARY_DIR "${EXTERNAL_DIR}/GLFW/install/lib")
            if (MSVC)
                set(GLFW_LIBRARIES "glfw3.lib")
            else()
                set(GLFW_LIBRARIES "libglfw3.a")
            endif()

            # When we install MinVR, install the new library to the same place so other projects
            # including future clean builds of MinVR can use it.  Note, install step is separate
            # from the one that is optionally included in ExternalProject_add.  If you setup the
            # external project to install, that install refers to whatever install system is
            # builtin to the external project's build system, whereas this install refers to the
            # MinVR library installation process.
            if (DEPENDENCIES_INSTALL)
                install(DIRECTORY ${GLFW_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
                # excluding cmake directory b/c its paths will be for the inside the external/ dir
                install(DIRECTORY ${GLFW_LIBRARY_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib PATTERN "cmake" EXCLUDE)
            endif()

        else()

            message(FATAL_ERROR "The GLFW library was not found on the system.  You can: (1) install GLFW yourself, (2)point cmake to an already-installed version of GLFW by adding the installation prefix of GLFW to the CMAKE_PREFIX_PATH variable, or (3) set DEPENDENCIES_AUTOBUILD to ON to have MinVR download and build GLFW in its 'external' directory for you.")

        endif()

    endif()

    message(STATUS "Adding build flags for GLFW programs.")

    add_definitions(-DUSE_GLFW)

    if (MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        # Tell MSVC to use main instead of WinMain for Windows subsystem executables
        set_target_properties(${WINDOWS_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        find_library(COCOA_LIB Cocoa)
        find_library(IOKIT_LIB IOKit)
        find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
        find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
        list(APPEND GLFW_LIBRARIES ${COCOA_LIB} ${IOKIT_LIB} ${CORE_FOUNDATION_FRAMEWORK} ${CORE_VIDEO_FRAMEWORK})
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        find_package(Threads)
        find_package(X11)
        list(APPEND GLFW_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xinerama Xcursor Xi m dl ${X11_LIBRARIES})
    endif()

endmacro()
