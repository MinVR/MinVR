# Create a library from the source files in this directory.

project (VRDisplay)

set (${PROJECT_NAME}_source_files 
	VRRenderState.cpp
	base/VRDisplayNode.cpp
	base/VRBasicRenderer.cpp
	base/VRFunctionRenderCallback.cpp
	console/VRConsoleDisplay.cpp
	graphics/VRGraphicsWindowChild.cpp
	graphics/VRGraphicsWindowNode.cpp
	graphics/stereo/VRStereoNode.cpp
	graphics/stereo/VRSideBySideNode.cpp
	graphics/structure/VRViewportNode.cpp
	graphics/structure/VRViewport.cpp
	graphics/structure/VRViewportCalculator.cpp
	graphics/structure/VRTile.cpp
	graphics/structure/VRTileNode.cpp
)

add_library (${PROJECT_NAME} ${${PROJECT_NAME}_source_files})

install(DIRECTORY ${PROJECT_SOURCE_DIR}  DESTINATION "include" FILES_MATCHING PATTERN "*.h")

install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION bin)
