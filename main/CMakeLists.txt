# Create a library from the source files in this directory.

project (VRMain)

set (${PROJECT_NAME}_source_files 
	VRMain.cpp
	VRMain.h
)

add_library (${PROJECT_NAME} ${${PROJECT_NAME}_source_files})
