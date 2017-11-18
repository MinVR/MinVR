# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

# Create some test programs from the source files in this directory.

## Run these tests with 'make test' or 'ctest -VV' if you want to see
## the output.  You can also do 'ctest --memcheck' that runs the tests
## with some memory checking enabled.

set (maintests utility)
# The numbers here correspond to 'case' statements in the respective
# test program.  See, e.g., utilitytest.cpp
set (utility_parts 1 2)

# For tests where a list of parts has not been defined we add a default of 1:
foreach(maintest ${maintests})
  if(NOT DEFINED "${maintest}_parts")
     set(${maintest}_parts "1")
  endif()
endforeach()

# Don't forget the .cpp files for each test:
foreach(maintest ${maintests})
  set(maintestsrc ${maintestsrc} ${maintest}test.cpp)
endforeach()

# Each of these .cpp files has a function with the same name as the
# file.

create_test_sourcelist(srclist RunSomeIndexTests.cpp ${maintestsrc})
add_executable(test-main ${srclist})
target_link_libraries(test-main MinVR)

# When it's compiled you can run the test-main executable and
# specify a particular test and subtest:
#./test-main queuetest 1
#All that's left is to tell CMake to generate the test cases:

set($ENV{MVRHOME} ${CMAKE_SOURCE_DIR})

foreach(maintest ${maintests})
  foreach(part ${${maintest}_parts})
    add_test(NAME test_${maintest}_${part}
      COMMAND ${CMAKE_BINARY_DIR}/bin/test-main ${maintest}test ${part}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests-batch/config)
    set_tests_properties(test_${maintest}_${part} PROPERTIES
      FAIL_REGULAR_EXPRESSION "ERROR;FAIL;Test failed")
  endforeach()
endforeach()
