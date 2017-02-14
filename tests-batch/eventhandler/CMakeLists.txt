# Create a library from the source files in this directory.

project (test_eventdata)

add_executable (test_eventhandler main.cpp VRWandMoveEvent.cpp VRWandMoveEvent.h)
target_link_libraries (test_eventhandler VRIndex VREvent VRMath VRNet)

