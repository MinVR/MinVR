# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

# Create some test programs from the source files in this directory.

## Run these tests with 'make test' or 'ctest -VV' if you want to see
## the output.  You can also do 'ctest --memcheck' that runs the tests
## with some memory checking enabled.
##
## If you want to run only the network tests, try a command like this:
## 'ctest -R test_network_2 -VV'.  The -R is a regex match, so you can use
## -R network_2 or something smaller, too.

add_definitions(-DBINARYPATH="${CMAKE_BINARY_DIR}")

set (networktests network)
# The numbers here correspond to 'case' statements in the respective
# test program.  See, e.g., datumtest.cpp
set (network_parts 1 2 3)

# Fix this to match the config version after the networktest binary works ok.
set(networktestsrc networktest.cpp)

create_test_sourcelist(srclist RunSomeNetworkTests.cpp ${networktestsrc})
add_executable(test-network ${srclist})
target_link_libraries(test-network MinVR)

# Helper programs for the test.  We use these to start up clients with
# whom to test communications.  Their need is related to the use of
# fork() and execl().  Apparently it's unwise to do anything with a
# fork substantially more complicated than issuing an execl.  So the
# tests fork, but the forks just start up these client programs.
add_executable(launchSwapClient launchSwapClient.cpp)
target_link_libraries(launchSwapClient MinVR)

add_executable(launchEventClient launchEventClient.cpp)
target_link_libraries(launchEventClient MinVR)


# When it's compiled you can run the test-network executable and
# specify a particular test and subtest:
#./test-network queuetest 1
#All that's left is to tell CMake to generate the test cases:

set($ENV{MVRHOME} ${CMAKE_SOURCE_DIR})

foreach(networktest ${networktests})
  foreach(part ${${networktest}_parts})
    add_test(NAME test_${networktest}_${part}
      COMMAND ${CMAKE_BINARY_DIR}/bin/test-network ${networktest}test ${part}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests-batch/config)
    set_tests_properties(test_${networktest}_${part} PROPERTIES
      FAIL_REGULAR_EXPRESSION "ERROR;FAIL;Test failed")
  endforeach()
endforeach()
