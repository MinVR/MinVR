# Create a library from the source files in this directory.

project (VRDisplay)

set (${PROJECT_NAME}_source_files 
	base/VRDisplayNode.cpp
	graphics/VRGraphicsWindowChild.cpp
	graphics/VRGraphicsWindowNode.cpp
	graphics/stereo/VRStereoFormatter.cpp
	graphics/stereo/VRSideBySideNode.cpp
	graphics/structure/VRViewportNode.cpp
	graphics/structure/VRViewport.cpp
	graphics/structure/VRViewportFormatter.cpp
)

add_library (${PROJECT_NAME} ${${PROJECT_NAME}_source_files})

install(DIRECTORY ${PROJECT_SOURCE_DIR}  DESTINATION "include" FILES_MATCHING PATTERN "*.h")

install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION bin)
