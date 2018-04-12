# To setup a nightly build and test on your system:
# 1. Copy this file to some location on your system like /dashboards/
# 2. Edit the first section below (look for EDIT HERE) to define a 
#    unique name for your CTEST_SITE and tell ctest where to do the 
#    checkouts and testing.
# 3. Test your setup by running manually with the command:
#    ctest -S /your/path/ctest-nightly.cmake -V
#       - If you are on windows, you need to add "-C Release" to the end of the above command in order to have the tests run.
# 4. Follow the instructions here to schedule ctest to run each night as a cron job:
#    https://cmake.org/Wiki/CMake_Scripting_Of_CTest#Setting_Up_Cron.2FScheduler
# 5. Check the results on our dashboard:
#    http://my.cdash.org/index.php?project=MinVR

# References:
# https://cmake.org/Wiki/CMake/Testing_With_CTest
# https://cmake.org/Wiki/CMake_Scripting_Of_CTest


# ---- EDIT HERE: THESE SETTINGS WILL CHANGE FOR YOUR SITE ----

# Change this to a string that uniquely identifies the system you are setting up for testing
set(CTEST_SITE mymachine.mysite.edu)

# Change this to something that makes sense on your system
#set(DASHBOARD_DIR "/dashboards")
#set(DASHBOARD_DIR "/scratch/dashboards")
set(DASHBOARD_DIR "$ENV{HOME}/tmp/dashboards")



# ---- DEFINE THE test_libminvr AND test_example_program MACROS USED IN THE NEXT SECTION ----

# This macro is for checking out, configuring, building, and installing libminvr itself
macro(test_libminvr)
    message("\nRunning test_libminvr() with CTEST_CONFIGURE_OPTIONS set to:")
    message("-- begin --")
    message("${CTEST_CONFIGURE_OPTIONS}")
    message("-- end --")

    set(CTEST_SOURCE_DIRECTORY "${DASHBOARD_DIR}/MinVR_${DASHBOARD_TEST_NAME}/src")
    set(CTEST_BINARY_DIRECTORY "${DASHBOARD_DIR}/MinVR_${DASHBOARD_TEST_NAME}/build")

    set(CTEST_TRACK "MinVR ${DASHBOARD_TEST_NAME}")
    set(CTEST_BUILD_NAME "${CMAKE_HOST_SYSTEM} ${CTEST_BUILD_CONFIGURATION} [${CTEST_GIT_BRANCH}]")

    set(CTEST_USE_LAUNCHERS 1)

    find_program(CTEST_GIT_COMMAND NAMES git)
    set(CTEST_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")
    if(NOT EXISTS "${CTEST_SOURCE_DIRECTORY}")
      set(CTEST_CHECKOUT_COMMAND "${CTEST_GIT_COMMAND} clone --single-branch --branch ${CTEST_GIT_BRANCH} ${CTEST_GIT_REPO} ${CTEST_SOURCE_DIRECTORY}")
    endif()

    # Step 1: ctest_start()
    message("\n1. ctest_start()")

    # This is a strange command.  The intent is to avoid accidental deletes, but, it fails if certain cmake files are
    # not found in the binary directory already, for example, if the last build failed, then this will fail the next
    # time through, which is quite a problem.  So, we will simply delete the binary directory instead.
    #ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
    file(REMOVE_RECURSE ${CTEST_BINARY_DIRECTORY})

    ctest_start("${CTEST_MODEL}" TRACK ${CTEST_TRACK})

    # Step 2: ctest_update()
    message("\n2. ctest_update()")
    ctest_update(RETURN_VALUE exitcode)
    ctest_submit(PARTS Update)
    if(NOT "${exitcode}" STREQUAL "-1")

        # Step 3: ctest_configure()
        message("\n3. ctest_configure()")
        ctest_configure(OPTIONS "${CTEST_CONFIGURE_OPTIONS}" RETURN_VALUE exitcode)
        ctest_submit(PARTS Configure)
        if("${exitcode}" STREQUAL "0")

            # Step 4: ctest_build()
            message("\n4. ctest_build()")
            ctest_build(TARGET install RETURN_VALUE exitcode)
            ctest_submit(PARTS Build)
            if("${exitcode}" STREQUAL "0")

                # Step 5: ctest_test()
                message("\n5. ctest_test()")
                ctest_test(RETURN_VALUE exitcode)
                ctest_submit(PARTS Test)

            endif()

        endif()

    endif()

    # to assist with debugging, save this script in the notes that get uploaded to the cdash server
    set(CTEST_NOTES_FILES "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}")
    ctest_submit(PARTS Notes)
endmacro()


# This macro is for configuring and building an example program that links with the
# libminvr installed via the previous macro.
macro(test_example_program)
    # When building with ctest, the install directory is one level deeper than when building in the normal MinVR source tree
    set(CTEST_CONFIGURE_OPTIONS
      ${CTEST_CONFIGURE_OPTIONS}
      -DCMAKE_PREFIX_PATH=../../../build/install
    )

    message("\nRunning test_example_program() with CTEST_CONFIGURE_OPTIONS set to:")
    message("-- begin --")
    message("${CTEST_CONFIGURE_OPTIONS}")
    message("-- end --")

    set(CTEST_SOURCE_DIRECTORY "${DASHBOARD_DIR}/MinVR_${DASHBOARD_TEST_NAME}/src/examples/${EXAMPLE_NAME}")
    set(CTEST_BINARY_DIRECTORY "${DASHBOARD_DIR}/MinVR_${DASHBOARD_TEST_NAME}/build/examples/${EXAMPLE_NAME}")

    set(CTEST_TRACK "${EXAMPLE_NAME} ${DASHBOARD_TEST_NAME}")
    set(CTEST_BUILD_NAME "${CMAKE_HOST_SYSTEM} ${CTEST_BUILD_CONFIGURATION} [${CTEST_GIT_BRANCH}]")

    set(CTEST_USE_LAUNCHERS 1)

    find_program(CTEST_GIT_COMMAND NAMES git)
    set(CTEST_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")
    # The source is already checked out since it is inside the tree created by the test_libminvr macro
    unset(CTEST_CHECKOUT_COMMAND)

    # Step 1: ctest_start()
    message("\n1. ctest_start()")

    # This is a strange command.  The intent is to avoid accidental deletes, but, it fails if certain cmake files are
    # not found in the binary directory already, for example, if the last build failed, then this will fail the next
    # time through, which is quite a problem.  So, we will simply delete the binary directory instead.
    #ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
    file(REMOVE_RECURSE ${CTEST_BINARY_DIRECTORY})

    ctest_start("${CTEST_MODEL}" TRACK ${CTEST_TRACK})

    # Step 2: ctest_update()
    message("\n2. ctest_update()")
    ctest_update(RETURN_VALUE exitcode)
    ctest_submit(PARTS Update)
    if(NOT "${exitcode}" STREQUAL "-1")

        # Step 3: ctest_configure()
        message("\n3. ctest_configure()")
        ctest_configure(OPTIONS "${CTEST_CONFIGURE_OPTIONS}" RETURN_VALUE exitcode)
        ctest_submit(PARTS Configure)
        if("${exitcode}" STREQUAL "0")

            # Step 4: ctest_build()
            message("\n4. ctest_build()")
            ctest_build(TARGET install RETURN_VALUE exitcode)
            ctest_submit(PARTS Build)
            if("${exitcode}" STREQUAL "0")

                # Step 5: ctest_test()
                message("\n5. ctest_test()")
                ctest_test(RETURN_VALUE exitcode)
                ctest_submit(PARTS Test)

            endif()

        endif()

    endif()

    # to assist with debugging, save this script in the notes that get uploaded to the cdash server
    set(CTEST_NOTES_FILES "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}")
    ctest_submit(PARTS Notes)
endmacro()



# ---- CALL THE MACROS TO TEST SEVERAL CONFIGURATIONS ----

# Common to all the configurations we'll test
set(CTEST_GIT_REPO https://github.com/MinVR/MinVR.git)
set(CTEST_GIT_BRANCH beta)

set(CTEST_MODEL "Experimental")

set(CTEST_BUILD_CONFIGURATION "Release")

if(NOT DEFINED CTEST_CMAKE_GENERATOR)
  if (WIN32)
    # Is this the best default?
    set(CTEST_CMAKE_GENERATOR "Visual Studio 14 2015 Win64")
  else()
    set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
  endif()
endif()



# -- TEST CONFIGURATION #1:  "WithDefaults" --

set(DASHBOARD_TEST_NAME "WithDefaults")
set(CTEST_CONFIGURE_OPTIONS "-DAUTOBUILD_EXECUTE_NOW=ON")
test_libminvr()

set(EXAMPLE_NAME CavePainting-Lite)
test_example_program()



# -- TEST CONFIGURATION #2:  "WithAllOn" --

set(DASHBOARD_TEST_NAME "WithAllOn")
set(CTEST_CONFIGURE_OPTIONS
  # This requires proprietary sw that only runs at some sites
  -DWITH_PLUGIN_SCALABLE=OFF

  # This is being dropped from the main repo
  #  -DWITH_PLUGIN_FREEGLUT=ON

  -DWITH_DOCUMENTATION=ON
  -DWITH_ITEST_OPENGL_FIXEDFUNCPIPELINE=ON
  -DWITH_ITEST_OPENGL_MULTITHREADED=ON
  -DWITH_ITEST_OPENGL_SHADERPIPELINE=ON
  -DWITH_ITEST_OPENGL_SHADERPIPELINE_WITH_API=ON
  -DWITH_PLUGIN_GLFW=ON
  -DWITH_PLUGIN_OPENGL=ON
  -DWITH_PLUGIN_OPENVR=ON
  -DWITH_PLUGIN_PYTHON=ON
  -DWITH_PLUGIN_THREADING=ON
  -DWITH_PLUGIN_TUIO=ON
  -DWITH_PLUGIN_VRPN=ON

  -DAUTOBUILD_EXECUTE_NOW=ON
)
test_libminvr()

set(EXAMPLE_NAME CavePainting-Lite)
test_example_program()

