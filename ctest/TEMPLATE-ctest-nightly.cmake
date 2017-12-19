# To setup a nightly build and test on your system:
# 1. Copy this file and ctest-common.cmake to some location on your system like /dashboards/
# 2. Edit the top few lines of this file to define a unique name for your CTEST_SITE and
#    tell ctest where to do the checkouts and testing.
# 3. Test your setup by running manually with the command:
#    ctest -S /your/path/ctest-nightly.cmake -V
# 4. Follow the instructions here to schedule ctest to run each night as a cron job:
#    https://cmake.org/Wiki/CMake_Scripting_Of_CTest#Setting_Up_Cron.2FScheduler
# 5. Check the results on our dashboard:
#    http://my.cdash.org/index.php?project=MinVR



# Change this to a string that uniquely identifies the system you are setting up for testing
set(CTEST_SITE mymachine.minvr.edu)

# Change this to something that makes sense on your system
#set(DASHBOARD_DIR "/dashboards")
#set(DASHBOARD_DIR "/scratch/dashboards")
set(DASHBOARD_DIR "$ENV{HOME}/tmp/dashboards")


# --------------------------------------------------------

# This script runs through the test suite twice.  The first time tests the build in the
# situation where the default options are used to configure MinVR.  The second time turns
# on all plugins and other optional apsects of the build.

# TEST 1:

# Run the build and tests once with default options (will not build any plugins)
set(DASHBOARD_TEST_NAME WithDefaults)
set(CTEST_CONFIGURE_OPTIONS "")
include(${CTEST_SCRIPT_DIRECTORY}/ctest-common.cmake)

# Examples are not part of the main MinVR build, so we use an additional script to test
# building examples that link with libMinVR
set(EXAMPLE_NAME CavePainting-Lite)
include(${CTEST_SCRIPT_DIRECTORY}/ctest-addexample.cmake)


# TEST 2:

# Run again with all plugins enabled
set(DASHBOARD_TEST_NAME WithAllOn)
set(CTEST_CONFIGURE_OPTIONS
  -DWITH_PLUGIN_SCALABLE=OFF

  -DWITH_DOCUMENTATION=ON
  -DWITH_ITEST_OPENGL_FIXEDFUNCPIPELINE=ON
  -DWITH_ITEST_OPENGL_MULTITHREADED=ON
  -DWITH_ITEST_OPENGL_SHADERPIPELINE=ON
  -DWITH_ITEST_OPENGL_SHADERPIPELINE_WITH_API=ON
  -DWITH_PLUGIN_FREEGLUT=ON
  -DWITH_PLUGIN_GLFW=ON
  -DWITH_PLUGIN_OPENGL=ON
  -DWITH_PLUGIN_OPENVR=ON
  -DWITH_PLUGIN_PYTHON=ON
  -DWITH_PLUGIN_THREADING=ON
  -DWITH_PLUGIN_TUIO=ON
  -DWITH_PLUGIN_VRPN=ON
)
include(${CTEST_SCRIPT_DIRECTORY}/ctest-common.cmake)

# Examples are not part of the main MinVR build, so we use an additional script to test
# building examples that link with libMinVR
set(EXAMPLE_NAME CavePainting-Lite)
include(${CTEST_SCRIPT_DIRECTORY}/ctest-addexample.cmake)

