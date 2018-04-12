# Author: Daniel F. Keefe
# Copyright 2017,2018, Daniel F. Keefe and Regents of the University of Minnesota
# All Rights Reserved.

include(AutoBuild)


# Usage:
# AutoBuild_use_package_GLEW(
#    # The 1st argument is required.  It is the name of the target you wish to link this dependency to.
#    my-program 
#
#    # The 2nd argument is required.  It impacts how dependencies are propogated through CMake.  You can treat
#    # this dependency as an INTERFACE, PUBLIC, or PRIVATE.  See CMake's docs on Imported Targets for details.
#    PUBLIC 
# )
#
macro(AutoBuild_use_package_GLEW YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)
    set(PACKAGE_NAME "GLEW")
    string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)

    AutoBuild_find_package_module_mode(${PACKAGE_NAME})

    if ("${AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER}}")
        if ("${AUTOBUILD_EXECUTE_NOW}")

            message(STATUS "AutoBuild: Beginning download, build, install sequence.")

            AutoBuild_download_project( 
                ${PACKAGE_NAME}
                URL https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.zip
            )

            AutoBuild_build_and_install_project(
                ${PACKAGE_NAME}
                build/cmake
                -D_OPENGL_LIB_PATH=${_OPENGL_LIB_PATH} -D_OPENGL_INCLUDE_PATH=${_OPENGL_INCLUDE_PATH}
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

        if (TARGET GLEW::GLEW)
            # No need to set include dirs; this uses the modern cmake imported targets, so they are set automatically
            target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} GLEW::GLEW)
        else()
            # If we found an old version of glew that doesn't provide the GLEW::GLEW imported target, this should work instead
            target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLEW_LIBRARIES})
            target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${GLEW_INCLUDE_DIR})
        endif()

        target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_${PACKAGE_NAME})
    endif()

endmacro()

