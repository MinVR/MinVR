# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

project (test_eventdata)

add_executable (test_eventdata main.cpp)
target_link_libraries (test_eventdata VRData VREvent VRMath)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-batch”)
