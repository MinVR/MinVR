# Create some test programs from the source files in this directory.

## Run these tests with 'make test' or 'ctest -VV' if you want to see
## the output.  You can also do 'ctest --memcheck' that runs the tests
## with some memory checking enabled.

set (networktests network)
# The numbers here correspond to 'case' statements in the respective
# test program.  See, e.g., datumtest.cpp
set (network_parts 1 2 3)

# Fix this to match the config version after the networktest binary works ok.
set(networktestsrc networktest.cpp)

create_test_sourcelist(srclist RunSomeNetworkTests.cpp ${networktestsrc})
add_executable(test-network ${srclist})
target_link_libraries(test-network MinVR)

# Helper program related to the use of fork() and execl().
add_executable(launchSwapClient launchSwapClient.cpp)
target_link_libraries(launchSwapClient MinVR)

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
