# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

# Create some test programs from the source files in this directory.

## Run these tests with 'make test' or 'ctest -VV' if you want to see
## the output.  You can also do 'ctest --memcheck' that runs the tests
## with some memory checking enabled.
##
## If you want to run just one test, try 'ctest -VV -R index_15'
##

set (dataindextests datum index queue)
# The numbers here correspond to 'case' statements in the respective
# test program.  See, e.g., datumtest.cpp
set (datum_parts 1 2 3 4 5 6 7 8 9 10)
set (index_parts 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16)
set (queue_parts 1 2 3 4 5 6)

# For tests where a list of parts has not been defined we add a default of 1:
foreach(dataindextest ${dataindextests})
  if(NOT DEFINED "${dataindextest}_parts")
     set(${dataindextest}_parts "1")
  endif()
endforeach()

# Don't forget the .cpp files for each test:
foreach(dataindextest ${dataindextests})
  set(dataindextestsrc ${dataindextestsrc} ${dataindextest}test.cpp)
endforeach()

# Each of these .cpp files has a function with the same name as the
# file.

create_test_sourcelist(srclist RunSomeIndexTests.cpp ${dataindextestsrc})
add_executable(test-dataindex ${srclist})
target_link_libraries(test-dataindex MinVR)

# When it's compiled you can run the test-dataindex executable and
# specify a particular test and subtest:
#./test-dataindex queuetest 1
#All that's left is to tell CMake to generate the test cases:

set($ENV{MVRHOME} ${CMAKE_SOURCE_DIR})

foreach(dataindextest ${dataindextests})
  foreach(part ${${dataindextest}_parts})
    add_test(NAME test_${dataindextest}_${part}
      COMMAND ${CMAKE_BINARY_DIR}/bin/test-dataindex ${dataindextest}test ${part}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests-batch/config)
    set_tests_properties(test_${dataindextest}_${part} PROPERTIES
      FAIL_REGULAR_EXPRESSION "ERROR;FAIL;Test failed")
  endforeach()
endforeach()
