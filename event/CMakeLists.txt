# Create a library from the source files in this directory.

project (VREvent)

set (${PROJECT_NAME}_source_files 
	VREvent.cpp
	VREvent.h
)

add_library (${PROJECT_NAME} ${${PROJECT_NAME}_source_files})
