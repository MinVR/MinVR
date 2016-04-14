# Create a library from the source files in this directory.

project (VRMain)

set (${PROJECT_NAME}_source_files 
	VRMain.cpp
	VRMain.h
)

add_definitions(-DPLUGINPATH="${CMAKE_BINARY_DIR}/plugins")
add_definitions(-DPLUGININSTALLPATH="${CMAKE_INSTALL_PREFIX}/plugins")

add_library (${PROJECT_NAME} ${${PROJECT_NAME}_source_files})

install(DIRECTORY ${PROJECT_SOURCE_DIR}  DESTINATION "include" FILES_MATCHING PATTERN "*.h")

install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION bin)