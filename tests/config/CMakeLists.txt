# Create some test programs from the source files in this directory.

set (dataindextests datum index queue)
# The numbers here correspond to 'case' statements in the respective
# test program.  See, e.g., datumtest.cpp
set (datum_parts 1 2 3 4 5 6 7 8 9)
set (index_parts 1 2 3 4 5)
set (queue_parts 1 2)

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
add_executable(RunSomeIndexTests ${srclist})
target_link_libraries(RunSomeIndexTests VRIndex)

# When it's compiled you can run the RunSomeIndexTests executable and
# specify a particular test and subtest:
#./RunSomeIndexTests queuetest 1
#All that's left is to tell CMake to generate the test cases:

set($ENV{MVRHOME} ${CMAKE_SOURCE_DIR})

foreach(dataindextest ${dataindextests})
  foreach(part ${${dataindextest}_parts})
    add_test(NAME test_${dataindextest}_${part}
      COMMAND ${CMAKE_BINARY_DIR}/bin/RunSomeIndexTests ${dataindextest}test ${part}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/config)
    set_tests_properties(test_${dataindextest}_${part} PROPERTIES
      FAIL_REGULAR_EXPRESSION "ERROR;FAIL;Test failed")
  endforeach()
endforeach()

# OK I found a possible solution. Adding these lines to my CMakeLists.txt:

# ## valgrind
# find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)
# INCLUDE(Dart)
# generates the needed DartConfiguration.tcl whatever that is, and then re-invoking:

# ctest -T memcheck
# runs all tests also checking for memory errors.



project (memtest)

add_executable (memtest memtest.cpp)
target_link_libraries (memtest VRIndex)


