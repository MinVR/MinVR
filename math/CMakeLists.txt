# Create a library from the source files in this directory.

project (VRMath)

set (${PROJECT_NAME}_source_files 
  VRMath.cpp
  VRMath.h
)

add_library (${PROJECT_NAME} ${${PROJECT_NAME}_source_files})
