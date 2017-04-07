# Create a library from the source files in this directory.

project (test_eventdata)

add_executable (test_eventdata main.cpp)
target_link_libraries (test_eventdata VRData VREvent VRMath)

