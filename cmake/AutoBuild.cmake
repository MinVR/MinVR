# Author: Daniel F. Keefe
# Copyright 2017,2018, Daniel F. Keefe and Regents of the University of Minnesota
# All Rights Reserved.


# OVERVIEW / INTENT:  AutoBuild is a series of CMake macros and functions for working with sub-projects that are dependencies of a main project.  We assume that both the main project and dependencies each build with CMake already.  AutoBuild manages the process of checking to see if the dependencies are already available, installed on the system.  If not, AutoBuild will automatically download, build, and install the dependencies for you as part of the CMake Configure process.

# WHY IS THIS NEEDED / USEFUL?:  AutoBuild is similar in spirit to CMake's ExternalProject_add() command.  In fact, AutoBuild uses that command internally.  However, there is a key difference between AutoBuild and ExternalProject_add().  With ExternalProject_add() there is a chicken-and-the-egg problem.  ExternalProject_add() doesn't download and build the external project until after CMake has finished running and you are building your actual code using Unix Makefiles, Visual Studio, or Xcode.  This means the dependency does not exist on disk until CMake has finished running, and, thus, there is no way to use CMake's find_package() routines to find the dependency.  You can still make your project link with the dependency, but you have to hardcode paths to tell CMake where it will exist after it is built rather than find it already installed on the system.  If you want to only build the dependency if it is not already found on the system, then this means you need to maintain two ways of linking with the dependency in your CMakeLists.txt, using one approach when the dependency is found already installed and using another approach (hardcoded paths with ExternalProject_add()) when it is not found.  The hardcoded paths can get a bit complex when working with cross-platform code, and this is precisely what CMake's find_package() is supposed to handle, so it is a bit annoying that it cannot be used in this case.

# HOW IT WORKS:  AutoBuild takes a different approach where the goal is for our project's CMakeLists.txt file to always use the same approach to link with a dependency.  If the dependency is found on the system, then we go ahead and link with it right away.  If it is not found, then rather than scheduling it to be built as part of the build process for our own project, we "pause" our build for a moment to download, build, and install the dependency.  Then, once the dependency has been installed, we try again to find it using find_package(), which should always succeed since we just did the install, and continue on building our program.

# LIMITATIONS: Handling dependencies is a hard problem, and this "AutoBuild" strategy is not without its own limitations.  For example, a complication arises when using the CMake GUI.  When using the CMake GUI, users expect to be able to set the CMAKE_INSTALL_PREFIX interactively after pressing Configure once to bring up all the available options.  So, there is a problem if we build and install dependency projects during this first run of CMake Configure -- the user will not yet have had a chance to set CMAKE_INSTALL_PREFIX before the dependency projects are installed, and they may be installed to an undesirable location.  AutoBuild addresses this by running a status check at the end of the CMake Configure step.  The first time the status check detects that a dependency project should be "auto-built", it prints an informative error message, reminding the user to set CMAKE_INSTALL_PREFIX and then re-run CMake Configure to actually do the download, build, and install steps for each dependency.  This seems to be a good match for CMake's GUI mode because it also allows the user to check other related settings (e.g., turning off autobuild for specific projects, forcing autobuild even if the dependency is found on the system, setting the directory to use for downloading projects).  

# RUNNING FROM A SCRIPT:  When running from the command line or a script, the error-the-first-time-an-autobuild-is-scheduled behavior described above is typically not desired.  In this case, users should already be setting CMAKE_INSTALL_PREFIX and any other desired CMake flags on the command line so it is fine to run the autobuilds immediately.  To do this, simply run cmake with the following command line argument: -DAUTOBUILD_EXECUTE_NOW=ON


# USAGE: WHAT TO ADD TO YOUR CMAKELISTS.TXT FILE TO USE AUTOBUILD:

# Edit 1: Add two calls to your main CMakeLists.txt: 

# 1a. Near the top of the file include AutoBuild and call init, like so:
#    include(AutoBuild)
#    AutoBuild_init()

# 1b. At the end of the file call:
#    AutoBuild_check_status()


# Edit 2: For each target you wish to link with an external dependency, add a use_package() call right after you
#         define the target with add_library() or add_executable.  If the dependency you wish to link with uses
#         modern CMake Config Packages, then the generic AutoBuild_use_package() call is all that is needed.  
#         For example, to link my-program with the MinVR library, add the following to the CMakeLists.txt file 
#         where you define the my-program target with add_executable:

#    add_execuatable(my-program ${SOURCEFILES})
#    AutoBuild_use_package(MinVR . my-program PUBLIC MinVR::MinVR
#        DOWNLOAD_ARGS  GIT_REPOSITORY https://github.com/MinVR/MinVR.git GIT_TAG beta
#    )

#        Since CMake Config Packages are relatively new and very complex, so not every package provides one
#        (or a correct one).  In these cases, the generic AutoBuild_use_package() will not work, and we have 
#        to write a version of that is specific to the project we wish to add as a dependency.  For example,
#        in order to link with GLFW, which does not provide a ConfigGLFW.cmake file, we must write a
#        GLFW-specific version of AutoBuild_use_package() and then use it like so:

#     include(AutoBuildGLFW)
#     AutoBuild_use_package_GLFW(my-program PUBLIC)

#        GLFW is one of the first examples in this style, so it is included in the AutoBuild repo.  If the
#        repo does not already include an AutoBuild<package-name>.cmake script, then copy an existing 
#        example, modify it to work for the package of interest, and once it is working, push it back to 
#        the AutoBuild repo so others can benefit as well.







# Usage:
# AutoBuild_init() 
# Call this one time in your main CMakeLists.txt script somewhere near the beginning of the file.
macro(AutoBuild_init)
    message(STATUS "AutoBuild: Initializing.")

    option(AUTOBUILD_DEPENDENCIES "This is a global option that turns on/off the AutoBuild functionality.  It is also possible to turn on/off autobuilding for specific packages via variables of the form AUTOBUILD_PACKAGE_<NAME>." ON)

    set(AUTOBUILD_DOWNLOAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external" CACHE STRING "Location for external project sources downloaded by AutoBuild." )
endmacro()






# Usage:
# AutoBuild_check_status() 
# Call this one time in your main CMakeLists.txt script somewhere near the end of the file.
macro(AutoBuild_check_status)
    message(STATUS "AutoBuild: Checking status...")

    # Report status to the user, and if we detected dependencies to autobuild, stop with a
    # cmake error to give the user a chance to adjust config options, such as the 
    # CMAKE_INSTALL_PREFIX and overriding AUTOBUILD_<PROJNAME> on a case-by-case basis if 
    # needed.  Assuming all looks good, we will then download, build, and install each 
    # dependency that we need the next time CMake Configure is run.

    set(PREFIX "AUTOBUILD_PACKAGE_")
    get_cmake_property(ALLVARS VARIABLES)
    string(REGEX MATCHALL "(^|;)${PREFIX}[A-Za-z0-9_]*" MATCHED_VARS "${ALLVARS}")

    if (NOT "${MATCHED_VARS}" STREQUAL "")
        message(STATUS "AutoBuild: Status check for each dependency linked using AutoBuild:")
    
        set(RERUN_CONFIGURE "FALSE")

        foreach (VARIABLE ${MATCHED_VARS})
            #message(STATUS "${VARIABLE}=${${VARIABLE}}")

            string(REPLACE ${PREFIX} "" PACKAGE_NAME ${VARIABLE})
            string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)

            if ("${${PACKAGE_NAME}_AUTOBUILT}")
                set(STATUS_STR "Now installed to CMAKE_INSTALL_PREFIX (${CMAKE_INSTALL_PREFIX}).")
            elseif("${${VARIABLE}}")
                set(STATUS_STR "Set to auto-build next time you run CMake Configure.")
                set(RERUN_CONFIGURE "TRUE")
            else()
                set(STATUS_STR "Autobuild off (either found on the system or manually turned off).")
            endif()        

            message(STATUS "AutoBuild: Dependency = ${PACKAGE_NAME};  Status = ${STATUS_STR}")

        endforeach()

        # Error out if any packages are set to autobuild but have not yet been built
        if ("${RERUN_CONFIGURE}")
            
            # This flag confirms that the user has had a chance to see any new AutoBuild config 
            # options and we can now proceed on the next run of CMake Configure to do the auto 
            # download, build, and install steps.
            set(AUTOBUILD_EXECUTE_NOW "ON" CACHE BOOL "If on, the AutoBuild download, build, install steps will run the next time CMake Configure is run.")
            mark_as_advanced(AUTOBUILD_EXECUTE_NOW)

            message(SEND_ERROR "AutoBuild: One or more packages are setup to be downloaded, built, and installed with AutoBuild the next time you run CMake Configure.  Check now to make sure that your CMAKE_INSTALL_PREFIX points to the place where you want these dependencies installed.  Also, check variables of the form AUTOBUILD_PACKAGE_* to make sure the packages you want to build are ON and the ones you do not are OFF.  When all looks good, run Configure again to perform the download, build, and install steps.  This message will be shown only once -- the first time your project schedules a dependency to be autobuilt.")

        endif()
    endif()

    message(STATUS "AutoBuild: Completed status check.")
endmacro()







# Usage:
# AutoBuild_use_package(
#    # This 1st argument is required, it is the name of the project/package you wish to use.  The project should
#    # provide a <package-name>Config.cmake file for CMake to find.
#    MinVR 
#
#    # The 2nd argument is required and it is the relative path from the root of the external project's source
#    # tree to a CMakeLists.txt file that can be used to build the project.
#    . 
#
#    # The 3rd argument is required.  It is the name of the target you wish to link this dependency to.
#    my-program 
#
#    # The 4th argument is required.  It impacts how dependencies are propogated through CMake.  You can treat
#    # this dependency as an INTERFACE, PUBLIC, or PRIVATE.  See  the following URL for details:
#    # https://cmake.org/cmake/help/latest/command/target_include_directories.html
#    PUBLIC 
#
#    # The 5th argument is required.  It is the name of the imported target(s) provided in the projects CMake
#    # Config Package.  By convention, these typically start with the project name and then ::, like a namespace.
#    MinVR::MinVR
#
#    # The remaining arguments are optional and are used to provide more specific instructions on the find_package,
#    # download, build, and install steps.  List additional flags for these steps after the keywords: 
#    # FIND_PACKAGE_ARGS, DOWNLOAD_ARGS, and/or BUILD_AND_INSTALL_ARGS.  For example:
#
#    DOWNLOAD_ARGS  GIT_REPOSITORY https://github.com/MinVR/MinVR.git GIT_TAG beta
# )
#
macro(AutoBuild_use_package PACKAGE_NAME RELPATH_TO_CMAKELISTS YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE IMPORTED_TARGETS)

    set(OPTIONS)
    set(ONE_VALUE_ARGS)
    set(MULTI_VALUE_ARGS FIND_PACKAGE_ARGS DOWNLOAD_ARGS BUILD_AND_INSTALL_ARGS)
    cmake_parse_arguments(EXTRA "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)

    AutoBuild_find_package_config_mode(${PACKAGE_NAME} ${EXTRA_FIND_PACKAGE_ARGS})

    if ("${AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER}}")
        if ("${AUTOBUILD_EXECUTE_NOW}")
            message(STATUS "AutoBuild: Starting download, build, install sequence for ${PACKAGE_NAME}.")

            AutoBuild_download_project(${PACKAGE_NAME} ${EXTRA_DOWNLOAD_ARGS})

            AutoBuild_build_and_install_project(${PACKAGE_NAME} ${RELPATH_TO_CMAKELISTS} ${EXTRA_BUILD_AND_INSTALL_ARGS})

            AutoBuild_find_built_package_config_mode(${PACKAGE_NAME} CONFIG ${EXTRA_FIND_PACKAGE_ARGS})

            set(${PACKAGE_NAME_UPPER}_AUTOBUILT "TRUE" CACHE BOOL "Confirms that package ${PACKAGE_NAME} was successfully built by the AutoBuild system.")
            mark_as_advanced(${PACKAGE_NAME}_AUTOBUILT)
        
        else()
            message(STATUS "AutoBuild: Scheduled to build ${PACKAGE_NAME} the next time CMake Configure is run.")
        endif()
    endif()
    

    # If we've found the package, either directly on the system or after autobuilding, then link with it
    if ("${${PACKAGE_NAME}_FOUND}" OR "${${PACKAGE_NAME_UPPER}_FOUND}")
        message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency ${PACKAGE_NAME}.")

        target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${IMPORTED_TARGETS})

        target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_${PACKAGE_NAME_UPPER})
    endif()

endmacro()











# ------ Implementation Details Below this Line -- External Users Should Not Need To Call Anything Below -------


macro(AutoBuild_find_package_config_mode PACKAGE_NAME)
    # Users can pass in extra arguments to the find_package command if needed.
    set(EXTRA_MACRO_ARGS ${ARGN})

    string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)


    # Force CMake to find the package again -- need for the case where the package exists on the system
    # but we want to force building and installing it.  after the install, we need to clear these flags
    # so that find_package will find the newlyl installed version.
    unset(${PACKAGE_NAME}_DIR CACHE)
    unset(${PACKAGE_NAME_UPPER}_DIR CACHE)
    unset(${PACKAGE_NAME}_FOUND CACHE)
    unset(${PACKAGE_NAME_UPPER}_FOUND CACHE)

    message(STATUS "AutoBuild: Searching for package ${PACKAGE_NAME} using cmake's find_package() config mode...")

    if (NOT AUTOBUILD_DEPENDENCIES)

        message(STATUS "AutoBuild: AUTOBUILD_DEPENDENCIES is OFF, using default CMake find_package() behavior.")
        find_package(${PACKAGE_NAME} CONFIG ${EXTRA_MACRO_ARGS})

    else()

        # Try first to find the package within the current CMAKE_INSTALL_PREFIX.  In the case when multiple
        # versions of a library are installed on the same system, this makes sure that we will find a version
        # that was previously built using AutoBuild first.
        find_package(${PACKAGE_NAME} CONFIG PATHS ${CMAKE_INSTALL_PREFIX} NO_DEFAULT_PATH ${EXTRA_MACRO_ARGS})

        # Try again with the normal version of find_package(), which will search a variety of places on
        # the system, see details here:  https://cmake.org/cmake/help/latest/command/find_package.html
        if (NOT ("${${PACKAGE_NAME}_FOUND}" OR "${${PACKAGE_NAME_UPPER}_FOUND}"))
            find_package(${PACKAGE_NAME} CONFIG ${EXTRA_MACRO_ARGS})
        endif()


        if ("${${PACKAGE_NAME}_FOUND}" OR "${${PACKAGE_NAME_UPPER}_FOUND}")
            set(FOUND_AT_DIR "${${PACKAGE_NAME}_DIR}")
            if ("${FOUND_AT_DIR}" STREQUAL "")
                set(FOUND_AT_DIR "${${PACKAGE_NAME_UPPER}_DIR}")
            endif()
            message(STATUS "AutoBuild: OK - Found ${PACKAGE_NAME} at ${FOUND_AT_DIR}.")
            # Found the package on the system, so there is no need to autobuild it, but we still add
            # an option (defaulting to OFF) so that users can "force" building their own version of
            # the dependency if they wish.
            option(AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER} "If on, the AutoBuild CMake plugin will download, build, and install ${PACKAGE_NAME} to CMAKE_INSTALL_PREFIX the next time that CMake Configure is run." OFF)
        else() 
            message(STATUS "AutoBuild: Could not find ${PACKAGE_NAME}, setting AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER}=ON.")
            # If the package is still not found, then we will need to autobuild it, add the option with a default of ON
            option(AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER} "If on, the AutoBuild CMake plugin will download, build, and install ${PACKAGE_NAME} to CMAKE_INSTALL_PREFIX the next time that CMake Configure is run." ON)
        endif()

    endif()
endmacro()



macro(AutoBuild_find_built_package_config_mode PACKAGE_NAME)
    # Users can pass in extra arguments to the find_package command if needed.
    set(EXTRA_MACRO_ARGS ${ARGN})

    string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)

    # Force CMake to find the package again -- need for the case where the package exists on the system
    # but we want to force building and installing it.  after the install, we need to clear these flags
    # so that find_package will find the newlyl installed version.
    unset(${PACKAGE_NAME}_DIR CACHE)
    unset(${PACKAGE_NAME_UPPER}_DIR CACHE)
    unset(${PACKAGE_NAME}_FOUND CACHE)
    unset(${PACKAGE_NAME_UPPER}_FOUND CACHE)

    message(STATUS "AutoBuild: Searching again (after autobuilding) for package ${PACKAGE_NAME} using config mode...")

    # Try first to find the package within the current CMAKE_INSTALL_PREFIX.  In the case when multiple
    # versions of a library are installed on the same system, this makes sure that we will find a version
    # that was previously built using AutoBuild first.
    find_package(${PACKAGE_NAME} PATHS ${CMAKE_INSTALL_PREFIX} NO_DEFAULT_PATH ${EXTRA_MACRO_ARGS} QUIET)

    # Try again with the normal version of find_package(), which will search a variety of places on
    # the system, see details here:  https://cmake.org/cmake/help/latest/command/find_package.html
    if (NOT ("${${PACKAGE_NAME}_FOUND}" OR "${${PACKAGE_NAME_UPPER}_FOUND}"))
        find_package(${PACKAGE_NAME} ${EXTRA_MACRO_ARGS})
    endif()


    if ("${${PACKAGE_NAME}_FOUND}" OR "${${PACKAGE_NAME_UPPER}_FOUND}")
        set(FOUND_AT_DIR "${${PACKAGE_NAME}_DIR}")
        if ("${FOUND_AT_DIR}" STREQUAL "")
            set(FOUND_AT_DIR "${${PACKAGE_NAME_UPPER}_DIR}")
        endif()
        message(STATUS "AutoBuild: OK - Found ${PACKAGE_NAME} at ${FOUND_AT_DIR}.")
    else()
        message(FATAL_ERROR "AutoBuild: Just completed a download, build, and install sequence for package ${PACKAGE_NAME}, and it should now be installed at the prefix ${CMAKE_INSTALL_PREFIX}, but cmake is still unable to find it with find_package().")
    endif()
endmacro()



macro(AutoBuild_find_package_module_mode PACKAGE_NAME)
    # Users can pass in extra arguments to the find_package command if needed.
    set(EXTRA_MACRO_ARGS ${ARGN})

    string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)


    # Force CMake to find the package again -- need for the case where the package exists on the system
    # but we want to force building and installing it.  after the install, we need to clear these flags
    # so that find_package will find the newlyl installed version.
    unset(${PACKAGE_NAME}_DIR CACHE)
    unset(${PACKAGE_NAME_UPPER}_DIR CACHE)
    unset(${PACKAGE_NAME}_FOUND CACHE)
    unset(${PACKAGE_NAME_UPPER}_FOUND CACHE)

    message(STATUS "AutoBuild: Searching for package ${PACKAGE_NAME} using cmake's find_package() module mode...")

    if (NOT AUTOBUILD_DEPENDENCIES)

        message(STATUS "AutoBuild: AUTOBUILD_DEPENDENCIES is OFF, using default CMake find_package() behavior.")
        find_package(${PACKAGE_NAME} MODULE ${EXTRA_MACRO_ARGS})

    else()

        # AutoBuild allows users to force building a dependency even if it is already found installed on the
        # system.  To make this work, any Find*.cmake scripts that are used with AutoBuild must look first
        # for the library under ${CMAKE_INSTALL_PREFIX}.  This makes it so that preference is given to
        # libraries just built and installed there with AutoBuild.
        find_package(${PACKAGE_NAME} MODULE ${EXTRA_MACRO_ARGS})

        if ("${${PACKAGE_NAME}_FOUND}" OR "${${PACKAGE_NAME_UPPER}_FOUND}")
            set(FOUND_AT_DIR "${${PACKAGE_NAME}_DIR}")
            if ("${FOUND_AT_DIR}" STREQUAL "")
                set(FOUND_AT_DIR "${${PACKAGE_NAME_UPPER}_DIR}")
            endif()
            message(STATUS "AutoBuild: OK - Found ${PACKAGE_NAME} at ${FOUND_AT_DIR}.")
            # Found the package on the system, so there is no need to autobuild it, but we still add
            # an option (defaulting to OFF) so that users can "force" building their own version of
            # the dependency if they wish.
            option(AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER} "If on, the AutoBuild CMake plugin will download, build, and install ${PACKAGE_NAME} to CMAKE_INSTALL_PREFIX the next time that CMake Configure is run." OFF)
        else() 
            message(STATUS "AutoBuild: Could not find ${PACKAGE_NAME}, setting AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER}=ON.")
            # If the package is still not found, then we will need to autobuild it, add the option with a default of ON
            option(AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER} "If on, the AutoBuild CMake plugin will download, build, and install ${PACKAGE_NAME} to CMAKE_INSTALL_PREFIX the next time that CMake Configure is run." ON)
        endif()

    endif()
endmacro()



macro(AutoBuild_find_built_package_module_mode PACKAGE_NAME)
    # Users can pass in extra arguments to the find_package command if needed.
    set(EXTRA_MACRO_ARGS ${ARGN})

    string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)

    # Force CMake to find the package again -- need for the case where the package exists on the system
    # but we want to force building and installing it.  after the install, we need to clear these flags
    # so that find_package will find the newlyl installed version.
    unset(${PACKAGE_NAME}_DIR CACHE)
    unset(${PACKAGE_NAME_UPPER}_DIR CACHE)
    unset(${PACKAGE_NAME}_FOUND CACHE)
    unset(${PACKAGE_NAME_UPPER}_FOUND CACHE)

    message(STATUS "AutoBuild: Searching again (after autobuilding) for package ${PACKAGE_NAME} using module mode...")

    # AutoBuild allows users to force building a dependency even if it is already found installed on the
    # system.  To make this work, any Find*.cmake scripts that are used with AutoBuild must look first
    # for the library under ${CMAKE_INSTALL_PREFIX}.  This makes it so that preference is given to
    # libraries just built and installed there with AutoBuild.
    find_package(${PACKAGE_NAME} MODULE ${EXTRA_MACRO_ARGS})

    if ("${${PACKAGE_NAME}_FOUND}" OR "${${PACKAGE_NAME_UPPER}_FOUND}")
        set(FOUND_AT_DIR "${${PACKAGE_NAME}_DIR}")
        if ("${FOUND_AT_DIR}" STREQUAL "")
            set(FOUND_AT_DIR "${${PACKAGE_NAME_UPPER}_DIR}")
        endif()
        message(STATUS "AutoBuild: OK - Found ${PACKAGE_NAME} at ${FOUND_AT_DIR}.")
    else()
        message(FATAL_ERROR "AutoBuild: Just completed a download, build, and install sequence for package ${PACKAGE_NAME}, and it should now be installed at the prefix ${CMAKE_INSTALL_PREFIX}, but cmake is still unable to find it with find_package().")
    endif()
endmacro()



# Usage:  
# AutoBuild_download_project(
#     # This first argument is the name of the project to download.  It is required:
#     glm
#
#     # Additional arguments specify how to download the project using GIT, SVN, CVS, or URL.
#     # These can be any of the arguments used for the downloading step of the cmake builtin
#     # ExternalProject_Add command. 
#     GIT_REPOSITORY "https://github.com/g-truc/glm.git"
#     GIT_TAG master
#     etc..
# )
function(AutoBuild_download_project EXT_PROJECT_NAME)

    big_message("AUTOBUILD: BEGIN EXTERNAL PROJECT DOWNLOAD (${EXT_PROJECT_NAME}).")

    message(STATUS "AutoBuild: Creating a download helper project for ${EXT_PROJECT_NAME}.")

    set(DOWNLOAD_OPTIONS ${ARGN})
    string (REGEX REPLACE "(^|[^\\\\]);" "\\1 " DOWNLOAD_OPTIONS "${DOWNLOAD_OPTIONS}")

    file(MAKE_DIRECTORY ${AUTOBUILD_DOWNLOAD_DIR}/${EXT_PROJECT_NAME})

    # Write the CMakeLists.txt.in file to disk
    set(DLHELPER_FILE_TEXT "\n \
        # This is a helper cmake project -- the only thing this project does is download \n \
        # the external project.  So, the configure, build, install, and test commands for \n \
        # ExternalProject_Add() are intentionally set as NOPs.  \n \ 
        \n \
        cmake_minimum_required (VERSION 3.9) \n \
        \n \
        project(@EXT_PROJECT_NAME@-download) \n \
        \n \
        include(ExternalProject) \n \
        ExternalProject_Add( \n \
            @EXT_PROJECT_NAME@ \n \
            SOURCE_DIR \"@AUTOBUILD_DOWNLOAD_DIR@/@EXT_PROJECT_NAME@/src\" \n \
            BINARY_DIR \"@AUTOBUILD_DOWNLOAD_DIR@/@EXT_PROJECT_NAME@/download-helper\" \n \
            @DOWNLOAD_OPTIONS@ \n \
            CONFIGURE_COMMAND \"\" \n \
            BUILD_COMMAND \"\" \n \
            INSTALL_COMMAND \"\" \n \
            TEST_COMMAND \"\" \n \
            LOG_DOWNLOAD ON \n \
            GIT_PROGRESS 1 \n \
        ) \n \
    ")

    file(WRITE ${AUTOBUILD_DOWNLOAD_DIR}/${EXT_PROJECT_NAME}/download-helper/CMakeLists.txt.in ${DLHELPER_FILE_TEXT})
    
    configure_file(
        ${AUTOBUILD_DOWNLOAD_DIR}/${EXT_PROJECT_NAME}/download-helper/CMakeLists.txt.in 
        ${AUTOBUILD_DOWNLOAD_DIR}/${EXT_PROJECT_NAME}/download-helper/CMakeLists.txt
    )

    message(STATUS "AutoBuild: Generating build files for the ${EXT_PROJECT_NAME} download helper project.")
    message(STATUS "  Using working dir: ${AUTOBUILD_DOWNLOAD_DIR}/${EXT_PROJECT_NAME}/download-helper")

    execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY "${AUTOBUILD_DOWNLOAD_DIR}/${EXT_PROJECT_NAME}/download-helper" OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr RESULT_VARIABLE exitcode)
    if(NOT "${exitcode}" STREQUAL "0")
        message(FATAL_ERROR "AutoBuild: Error while creating download helper project: ${stdout} ${stderr}")
    endif()

    message(STATUS "AutoBuild: Building the ${EXT_PROJECT_NAME} download helper project.  (This actually performs the download and may take some time...)")
    execute_process(COMMAND "${CMAKE_COMMAND}" --build . WORKING_DIRECTORY "${AUTOBUILD_DOWNLOAD_DIR}/${EXT_PROJECT_NAME}/download-helper" OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr RESULT_VARIABLE exitcode)
    if(NOT "${exitcode}" STREQUAL "0")
        message(FATAL_ERROR "AutoBuild: Error running download helper project: ${stdout} ${stderr}")
    endif()

    message(STATUS "AutoBuild: Completed download of external project ${EXT_PROJECT_NAME}.")

endfunction()


# Usage:  
# AutoBuild_build_and_install_project(
#     # This first argument is the name of the external project to download.  It is required:
#     VRPN
#     # This second argument is the relative path from ${EXTERNAL_DIR_NAME}/projectname/ to the project's
#     # main CMakeLists.txt file:
#     src
#
#     # Additional arguments are passed on as options to the cmake build file generator
#     -DVRPN_BUILD_DIRECTSHOW_VIDEO_SERVER=OFF
#     -DVRPN_BUILD_HID_GUI=OFF
#     etc..
# )
function(AutoBuild_build_and_install_project EXT_PROJECT_NAME RELPATH_TO_CMAKELISTS)

    big_message("AUTOBUILD: BEGIN EXTERNAL PROJECT BUILD AND INSTALL (${EXT_PROJECT_NAME}).")

    # any extra args to the function are interpreted as arguments for the cmake config process
    set(CMAKE_CONFIG_OPTIONS ${ARGN})

    # always set the install prefix to be the same as for the main project
    list(APPEND CMAKE_CONFIG_OPTIONS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})

    set(SRC_DIR "${AUTOBUILD_DOWNLOAD_DIR}/${EXT_PROJECT_NAME}/src/${RELPATH_TO_CMAKELISTS}")
    set(BUILD_DIR "${CMAKE_BINARY_DIR}/external/${EXT_PROJECT_NAME}")

    file(MAKE_DIRECTORY ${BUILD_DIR})

    message(STATUS "AutoBuild: Generating build files for external project ${EXT_PROJECT_NAME}.")
    message(STATUS "  Using source dir: ${SRC_DIR}")
    message(STATUS "  Using build dir: ${BUILD_DIR}")
    message(STATUS "  Config options: ${CMAKE_CONFIG_OPTIONS}")

    execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" ${SRC_DIR} ${CMAKE_CONFIG_OPTIONS} WORKING_DIRECTORY ${BUILD_DIR} OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr RESULT_VARIABLE exitcode)
    if(NOT "${exitcode}" STREQUAL "0")
        message(FATAL_ERROR "AutoBuild: Error generating build files for external project: ${stdout} ${stderr}")
    endif()

    message(STATUS "AutoBuild: Building external project ${EXT_PROJECT_NAME} (Configuration = Debug).  (This may take some time...)")
    execute_process(COMMAND "${CMAKE_COMMAND}" --build ${BUILD_DIR} --config Debug --target install OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr RESULT_VARIABLE exitcode)
    if(NOT "${exitcode}" STREQUAL "0")
        message(FATAL_ERROR "AutoBuild: Error building Debug configuration for external project: ${stdout} ${stderr}")
    endif()

    message(STATUS "AutoBuild: Building external project ${EXT_PROJECT_NAME} (Configuration = Release).  (This may take some time...)")
    execute_process(COMMAND "${CMAKE_COMMAND}" --build ${BUILD_DIR} --config Release --target install OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr RESULT_VARIABLE exitcode)
    if(NOT "${exitcode}" STREQUAL "0")
        message(FATAL_ERROR "AutoBuild: Error building Release configuration for external project: ${stdout} ${stderr}")
    endif()

    message(STATUS "AutoBuild: Completed external build of ${EXT_PROJECT_NAME}.\n\n")

endfunction()


macro(big_message TITLE)
  string(TOUPPER ${TITLE} TITLE)
  message(STATUS "\n\n==== ${TITLE} ====")
endmacro()



