# Create a library from the source files in this directory.



set(${PROJECT_NAME}_source_files
	${${PROJECT_NAME}_source_files}
  	${CMAKE_CURRENT_SOURCE_DIR}/VRConsoleNode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VRDisplayNode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VRGraphicsWindowNode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VRGroupNode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VROffAxisProjectionNode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VRStereoNode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VRViewportNode.cpp
	PARENT_SCOPE
 )