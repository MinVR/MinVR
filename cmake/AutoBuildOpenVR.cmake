# Author: Daniel F. Keefe
# Copyright 2017,2018, Daniel F. Keefe and Regents of the University of Minnesota
# All Rights Reserved.

include(AutoBuild)


# Usage:
# AutoBuild_use_package_OpenVR(
#    # The 1st argument is required.  It is the name of the target you wish to link this dependency to.
#    my-program 
#
#    # The 2nd argument is required.  It impacts how dependencies are propogated through CMake.  You can treat
#    # this dependency as an INTERFACE, PUBLIC, or PRIVATE.  See CMake's docs on Imported Targets for details.
#    PUBLIC 
# )
#
macro(AutoBuild_use_package_OpenVR YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)
    set(PACKAGE_NAME "OpenVR")
    string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)

    AutoBuild_find_package_module_mode(${PACKAGE_NAME})

    if ("${AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER}}")
        if ("${AUTOBUILD_EXECUTE_NOW}")

            message(STATUS "AutoBuild: Beginning download, build, install sequence.")

            AutoBuild_download_project( 
                ${PACKAGE_NAME}
                GIT_REPOSITORY https://github.com/ivlab/openvr
                GIT_TAG master
            )

            AutoBuild_build_and_install_project(
                ${PACKAGE_NAME}
                .
                -DBUILD_SHARED=ON
            )

            # OpenVR's install target seems to only install the lib.  So, install headers manually here.
            file(INSTALL ${AUTOBUILD_DOWNLOAD_DIR}/OpenVR/src/headers/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/openvr)
            file(INSTALL ${CMAKE_INSTALL_PREFIX}/lib/openvr_api64.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin OPTIONAL)
            file(INSTALL ${CMAKE_INSTALL_PREFIX}/lib/openvr_api.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin OPTIONAL)

            AutoBuild_find_built_package_module_mode(${PACKAGE_NAME})

            set(${PACKAGE_NAME_UPPER}_AUTOBUILT "TRUE" CACHE BOOL "Confirms that package ${PACKAGE_NAME} was successfully built by the AutoBuild system.")
            mark_as_advanced(${PACKAGE_NAME}_AUTOBUILT)

        else()
            message(STATUS "AutoBuild: Scheduled to build ${PACKAGE_NAME} the next time CMake Configure is run.")
        endif()
    endif()


    # If we've found the package, either directly on the system or after autobuilding, then link with it
    if ("${${PACKAGE_NAME}_FOUND}" OR "${${PACKAGE_NAME_UPPER}_FOUND}")
        message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency ${PACKAGE_NAME}.")

        target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${OPENVR_LIBRARIES})
        target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${OPENVR_INCLUDE_DIR})

        if (MSVC)
            add_definitions(-D_CRT_SECURE_NO_WARNINGS)
            # Tell MSVC to use main instead of WinMain for Windows subsystem executables
            set_target_properties(${WINDOWS_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
        endif()

        if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            find_library(COCOA_LIB Cocoa)
            target_link_libraries(${YOUR_TARGET} PRIVATE ${COCOA_LIB})
            find_library(IOKIT_LIB IOKit)
            target_link_libraries(${YOUR_TARGET} PRIVATE ${IOKIT_LIB})
            find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
            target_link_libraries(${YOUR_TARGET} PRIVATE ${CORE_FOUNDATION_FRAMEWORK})
            find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
            target_link_libraries(${YOUR_TARGET} PRIVATE ${CORE_VIDEO_FRAMEWORK})
        endif()

        if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
            find_package(Threads)
            find_package(X11)
            target_link_libraries(${YOUR_TARGET} PRIVATE ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xinerama Xcursor Xi m dl ${X11_LIBRARIES})
        endif()

        target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_${PACKAGE_NAME})
    endif()

endmacro()

