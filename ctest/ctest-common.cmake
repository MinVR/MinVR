# See the ctest-nightly.cmake file for instructions.  It includes this file.

# References:
# https://cmake.org/Wiki/CMake/Testing_With_CTest
# https://cmake.org/Wiki/CMake_Scripting_Of_CTest

# Our dashboard page:
# http://my.cdash.org/index.php?project=MinVR


message("\nRunning script in ctest-common.cmake with CTEST_CONFIGURE_OPTIONS set to:")
message("-- begin --")
message("${CTEST_CONFIGURE_OPTIONS}")
message("-- end --")


# Set options to defaults if not already defined by the calling script

if(NOT DEFINED DASHBOARD_TEST_NAME)
  set(DASHBOARD_TEST_NAME "Default")
endif()

if(NOT DEFINED DASHBOARD_DIR)
  set(DASHBOARD_DIR "$ENV{HOME}/tmp/dashboards")
endif()

if(NOT DEFINED CTEST_GIT_REPO)
  set(CTEST_GIT_REPO https://github.com/MinVR/MinVR.git)
endif()

if(NOT DEFINED CTEST_GIT_BRANCH)
  set(CTEST_GIT_BRANCH beta)
endif()

if(NOT DEFINED CTEST_MODEL)
  # Nightly, Continous, or Experimental
  set(CTEST_MODEL "Nightly")
endif()

if(NOT DEFINED CTEST_SITE)
    cmake_host_system_information(RESULT HOSTNAME QUERY HOSTNAME)
    set(CTEST_SITE "${HOSTNAME}")
endif()

if(NOT DEFINED CTEST_BUILD_CONFIGURATION)
  set(CTEST_BUILD_CONFIGURATION "Release")
endif()




# Define names based on the options above

set(CTEST_SOURCE_DIRECTORY "${DASHBOARD_DIR}/MinVR_${DASHBOARD_TEST_NAME}/src")
set(CTEST_BINARY_DIRECTORY "${DASHBOARD_DIR}/MinVR_${DASHBOARD_TEST_NAME}/build")

set(CTEST_TRACK "${CTEST_GIT_BRANCH}_${DASHBOARD_TEST_NAME}")

# This would be a more typical build name, but since we can't set a custom track without
# paying for a pro version of CDash, we add the branch and test name to the build name.
# set(CTEST_BUILD_NAME "${CMAKE_HOST_SYSTEM}_${CTEST_BUILD_CONFIGURATION}")
set(CTEST_BUILD_NAME "${CTEST_GIT_BRANCH}_MinVR_${DASHBOARD_TEST_NAME}_${CMAKE_HOST_SYSTEM}_${CTEST_BUILD_CONFIGURATION}")    




# Step 1: ctest_start()
message("\n1. ctest_start()")

find_program(CTEST_GIT_COMMAND NAMES git)

# do a checkout if not already there
if(NOT EXISTS "${CTEST_SOURCE_DIRECTORY}")
  set(CTEST_CHECKOUT_COMMAND "${CTEST_GIT_COMMAND} clone --single-branch --branch ${CTEST_GIT_BRANCH} ${CTEST_GIT_REPO} ${CTEST_SOURCE_DIRECTORY}")
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

ctest_configure(OPTIONS "${CTEST_CONFIGURE_OPTIONS}")
ctest_submit(PARTS Configure)



# Step 4: ctest_build()
message("\n4. ctest_build()")
if (WIN32)
  ctest_build(TARGET INSTALL)
else()
  ctest_build(TARGET install)
endif()
ctest_submit(PARTS Build)



# Step 5: ctest_test()
message("\n5. ctest_test()")
ctest_test()
ctest_submit(PARTS Test)



# to assist with debugging, save this script in the notes that get uploaded to the cdash server
set(CTEST_NOTES_FILES "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}")
ctest_submit(PARTS Notes)
