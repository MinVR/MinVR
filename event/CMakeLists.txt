# Create a library from the source files in this directory.

project (VREvent)

set (${PROJECT_NAME}_source_files 
	VREvent.cpp
	VREvent.h
	VREventQueue.h
	VREventQueue.cpp
	VREventFactory.h
	VREventFactory.cpp

)

#add_library (${PROJECT_NAME} ${${PROJECT_NAME}_source_files})

install(DIRECTORY ${PROJECT_SOURCE_DIR}  DESTINATION "include" FILES_MATCHING PATTERN "*.h")
