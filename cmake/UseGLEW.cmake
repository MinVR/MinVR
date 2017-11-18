# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Either finds a pre-installed version or downloads and builds the GLEW library.
#
# Usage: In your CMakeLists.txt:
# 1. Somewhere before your add_executable() or add_library() line, add the following:
#        include(UseGLEW)
#        UseGLEW()
# 2. After your add_executable() or add_library() line, add the following:
#        if (${GLEW_AUTOBUILT})
#            add_dependencies(${PROJECT_NAME} GLEW)
#        endif()
#
# Effect: This will use find_package() to try to find a version of GLEW already on 
# the system.  If found, that pre-installed version will be used to build the target.
# If no pre-installed version is found, then a new target will be added to the current
# build in order to download and build GLEW as an external project.  In this case, the
# same flags that find_package() sets (GLEW_INCLUDE_DIR, etc.) will be set to point to
# the newly build version so the rest of your CMakeLists.txt files can treat this as if
# the find_package() step were successful.  Finally, if you eventually install your own
# project, then cmake will also install the newly built version of GLEW to the same
# install prefix as for your project.


macro(UseGLEW)

    message(STATUS "Searching for GLEW library...")

	# Check to see if the library is already installed on the system
	find_package(GLEW)

    if (NOT ${GLEW_FOUND})
        message(STATUS "Ok: GLEW Found.")
    else()

    	# Either autobuild it or provide an informative error message
	    if (DEPENDENCIES_AUTOBUILD)

            set(GLEW_AUTOBUILT TRUE)

	        message(STATUS "Ok: AUTOBUILD_PLUGIN_DEPENDENCIES is ON so GLEW will be downloaded into the external directory and built for you.")

            include(ExternalProject)
            ExternalProject_add(GLEW
                PREFIX ${EXTERNAL_DIR}/GLEW
                URL https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.zip
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/GLEW/install ${GLEW_CMAKE_ARGS}
                HEADER_ONLY True
                SOURCE_SUBDIR build/cmake
            )

            # Set the same variables that find_package would
            set(GLEW_INCLUDE_DIR "${EXTERNAL_DIR}/GLEW/install/include")
            set(GLEW_LIBRARY_DIR "${EXTERNAL_DIR}/GLEW/install/lib")
            set(GLEW_BIN_DIR "${EXTERNAL_DIR}/GLEW/install/bin")

            if (MSVC)
                set(GLEW_LIBRARIES optimized GLEW.lib debug GLEWd.lib)
            else()
                set(GLEW_LIBRARIES optimized libGLEW.a debug libGLEWd.a)
            endif()


            # When we install MinVR, install the new library to the same place so other projects
            # including future clean builds of MinVR can use it.  Note, install step is separate
            # from the one that is optionally included in ExternalProject_add.  If you setup the
            # external project to install, that install refers to whatever install system is
            # builtin to the external project's build system, whereas this install refers to the
            # MinVR library installation process.
            if (DEPENDENCIES_INSTALL)
                install(DIRECTORY ${GLEW_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
                # excluding cmake directory b/c its paths will be for the inside the external/ dir
                install(DIRECTORY ${GLEW_LIBRARY_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib PATTERN "cmake" EXCLUDE)
                install(DIRECTORY ${GLEW_BIN_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
            endif()

	    else()

	        message(FATAL_ERROR "The GLEW library was not found on the system.  You can: (1) install GLEW yourself, (2) point cmake to an already-installed version of GLFW by adding the installation prefix of GLEW to the CMAKE_PREFIX_PATH variable, or (3) set AUTOBUILD_PLUGIN_DEPENDENCIES to ON to have MinVR download and build GLFW in its 'external' directory for you.")

	    endif()

	endif()

    message(STATUS "Adding build flags for GLEW programs.")

	add_definitions(-DUSE_GLEW)

    if (MSVC)
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    endif()

endmacro()
