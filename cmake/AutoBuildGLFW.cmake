# Author: Daniel F. Keefe
# Copyright 2017,2018, Daniel F. Keefe and Regents of the University of Minnesota
# All Rights Reserved.

include(AutoBuild)


# GLFW does export a cmake config package, but it doesn't seem to be setup correctly to work
# with imported targets.  So, the MODULE flag to find_package() below is critical -- it forces
# cmake to use our FindGLFW.cmake script rather than the glfwConfig.cmake package installed
# with GLFW.  If they fix their glfwConfig.cmake package at some point, then we can upgrade
# to just using the generic version of AutoBuild_use_package()



# Usage:
# AutoBuild_use_package_GLFW(
#    # The 1st argument is required.  It is the name of the target you wish to link this dependency to.
#    my-program 
#
#    # The 2nd argument is required.  It impacts how dependencies are propogated through CMake.  You can treat
#    # this dependency as an INTERFACE, PUBLIC, or PRIVATE.  See CMake's docs on Imported Targets for details.
#    PUBLIC 
# )
#
macro(AutoBuild_use_package_GLFW YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)
    set(PACKAGE_NAME "GLFW")
    string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)

    AutoBuild_find_package_module_mode(${PACKAGE_NAME})

    if ("${AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER}}")
        if ("${AUTOBUILD_EXECUTE_NOW}")

            message(STATUS "AutoBuild: Beginning download, build, install sequence.")

            AutoBuild_download_project( 
                ${PACKAGE_NAME}
                GIT_REPOSITORY https://github.com/ivlab/glfw.git
                GIT_TAG gpu-affinity
            )

            AutoBuild_build_and_install_project(
                ${PACKAGE_NAME}
                .
            )

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

        target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLFW_INCLUDE_DIR})
        target_link_libraries(${YOUR_TARGET} 
            ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLFW_LIBRARIES}
            ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLFW_cocoa_LIBRARY}
            ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLFW_corevideo_LIBRARY}
            ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLFW_iokit_LIBRARY}
        )

        if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
            find_package(Threads)
            find_package(X11)
            target_link_libraries(${YOUR_TARGET} 
                ${INTERFACE_PUBLIC_OR_PRIVATE} rt
                ${INTERFACE_PUBLIC_OR_PRIVATE} Xrandr
                ${INTERFACE_PUBLIC_OR_PRIVATE} Xxf86vm
                ${INTERFACE_PUBLIC_OR_PRIVATE} Xi
                ${INTERFACE_PUBLIC_OR_PRIVATE} m
                ${INTERFACE_PUBLIC_OR_PRIVATE} ${X11_LIBRARIES}
            )
        endif()

        target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_${PACKAGE_NAME})
    endif()

endmacro()

