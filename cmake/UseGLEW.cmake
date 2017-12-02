# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.
#
# Either finds a pre-installed version or downloads and builds the GLEW library.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the GLEW library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseGLEW)
#    UseGLEW(${PROJECT_NAME} PUBLIC)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html

# Effect: This will use find_package() to try to find a version of GLEW already on 
# the system.  If found, that pre-installed version will be used to build the target.
# If no pre-installed version is found, then a new target will be added to the current
# build in order to download and build GLEW as an external project.  In this case, the
# same flags that find_package() sets (GLEW_INCLUDE_DIR, etc.) will be set to point to
# the newly build version so the rest of your CMakeLists.txt files can treat this as if
# the find_package() step were successful.  In either case, GLEW will be linked to your
# project using target_include_directories() and target_link_libraries().  Finally, 
# if you eventually install your project, then cmake will also install the newly built
# version of GLEW to the same install prefix as for your project.

macro(UseGLEW YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    message(STATUS "Searching for GLEW library...")

	# Check to see if the library is already installed on the system
    # CMake ships with FindGLEW.cmake, which defines the GLEW::GLEW imported target
    # https://cmake.org/cmake/help/v3.9/module/FindGLEW.html

	find_package(GLEW)

    if (${GLEW_FOUND})
        message(STATUS "Ok: GLEW Found.")

        message(STATUS "Linking imported target GLEW::GLEW to target ${YOUR_TARGET}.")
        target_link_libraries(${YOUR_TARGET} GLEW::GLEW)
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
            add_dependencies(${YOUR_TARGET} GLEW)

            # Set the same variables that find_package would
            set(GLEW_INCLUDE_DIR "${EXTERNAL_DIR}/GLEW/install/include")
            set(GLEW_LIBRARY_DIR "${EXTERNAL_DIR}/GLEW/install/lib")
            set(GLEW_BIN_DIR "${EXTERNAL_DIR}/GLEW/install/bin")
            if (MSVC)
                set(GLEW_LIBRARIES optimized ${GLEW_LIBRARY_DIR}/GLEW.lib debug ${GLEW_LIBRARY_DIR}/GLEWd.lib)
            else()
                set(GLEW_LIBRARIES optimized ${GLEW_LIBRARY_DIR}/libGLEW.a debug ${GLEW_LIBRARY_DIR}/libGLEWd.a)
            endif()


            # When we install MinVR, install the new library to the same place so other projects
            # including future clean builds of MinVR can use it.  Note, install step is separate
            # from the one that is optionally included in ExternalProject_add.  If you setup the
            # external project to install, that install refers to whatever install system is
            # builtin to the external project's build system, whereas this install refers to the
            # MinVR library installation process.
            if (DEPENDENCIES_INSTALL)
                install(DIRECTORY ${GLEW_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
                install(DIRECTORY ${GLEW_LIBRARY_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
                install(DIRECTORY ${GLEW_BIN_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
            endif()

            target_link_libraries(${YOUR_TARGET} ${GLEW_LIBRARIES})
            target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} 
                $<BUILD_INTERFACE:${GLEW_INCLUDE_DIR}> # for headers when building
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>  # for client in install mode
            )

	    else()

	        message(FATAL_ERROR "The GLEW library was not found on the system.  You can: (1) install GLEW yourself, (2) point cmake to an already-installed version of GLFW by adding the installation prefix of GLEW to the CMAKE_PREFIX_PATH variable, or (3) set AUTOBUILD_PLUGIN_DEPENDENCIES to ON to have MinVR download and build GLFW in its 'external' directory for you.")

	    endif()

	endif()

    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_GLEW)

endmacro()
