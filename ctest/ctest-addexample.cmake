# See the ctest-nightly.cmake file for instructions.  It includes this file.

# After doing a build with ctest-common.cmake, include this file to also test
# building one of the apps in the examples directory.


message("\nRunning script in ctest-addexample.cmake for examples/${EXAMPLE_NAME}")

set(CTEST_SOURCE_DIRECTORY "${DASHBOARD_DIR}/MinVR_${DASHBOARD_TEST_NAME}/src/examples/${EXAMPLE_NAME}")
set(CTEST_BINARY_DIRECTORY "${DASHBOARD_DIR}/MinVR_${DASHBOARD_TEST_NAME}/build/examples/${EXAMPLE_NAME}")

set(CTEST_TRACK "${CTEST_GIT_BRANCH}_${EXAMPLE_NAME}_${DASHBOARD_TEST_NAME}")

# This would be a more typical build name, but since we can't set a custom track without
# paying for a pro version of CDash, we add the branch and test name to the build name.
# set(CTEST_BUILD_NAME "${CMAKE_HOST_SYSTEM}_${CTEST_BUILD_CONFIGURATION}")
set(CTEST_BUILD_NAME "${CTEST_GIT_BRANCH}_${EXAMPLE_NAME}_${DASHBOARD_TEST_NAME}_${CMAKE_HOST_SYSTEM}_${CTEST_BUILD_CONFIGURATION}")    


# Step 1: ctest_start()
message("\n1. ctest_start()")

find_program(CTEST_GIT_COMMAND NAMES git)

unset(CTEST_CHECKOUT_COMMAND)

if(NOT EXISTS "${CTEST_SOURCE_DIRECTORY}")
  message(FATAL_ERROR "The ctest-addexample.cmake script assumes the examples/${EXAMPLE_NAME} source was already checked out by a ctest-common.cmake script called earlier.")
endif()

ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

# Custom tracks requires the paid version of CDash
# ctest_start("${CTEST_MODEL}" TRACK ${CTEST_TRACK})
ctest_start("${CTEST_MODEL}")



# Step 2: ctest_update()
message("\n2. ctest_update()")

set(CTEST_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")
ctest_update()
ctest_submit(PARTS Update)



# Step 3: ctest_configure()
message("\n3. ctest_configure()")

if(NOT DEFINED CTEST_CMAKE_GENERATOR)
  if (WIN32)
    # Is this the best default?  Perhaps, assuming VS6 project files can be read into all later versions.
    set(CTEST_CMAKE_GENERATOR "Visual Studio 6")
  else()
    set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
  endif()
endif()

# When building with ctest, the install directory is one level deeper than when building in the normal MinVR source tree
set(CTEST_CONFIGURE_OPTIONS
  ${CTEST_CONFIGURE_OPTIONS}
  -DCMAKE_PREFIX_PATH=../../../build/install
)


ctest_configure(OPTIONS "${CTEST_CONFIGURE_OPTIONS}")
ctest_submit(PARTS Configure)



# Step 4: ctest_build()
message("\n4. ctest_build()")
ctest_build()
ctest_submit(PARTS Build)



# Step 5: ctest_test()
message("\n5. ctest_test()")
ctest_test()
ctest_submit(PARTS Test)



# to assist with debugging, save this script in the notes that get uploaded to the cdash server
set(CTEST_NOTES_FILES "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}")
ctest_submit(PARTS Notes)
