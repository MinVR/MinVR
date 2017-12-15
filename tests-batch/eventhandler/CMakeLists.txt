# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

project (test_eventdata)

add_executable (test_eventhandler main.cpp VRWandMoveEvent.cpp VRWandMoveEvent.h)
target_link_libraries (test_eventhandler VRIndex VREvent VRMath VRNet)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-batch”)
