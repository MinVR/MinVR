# Create a library from the source files in this directory.

project (VRPlugin)

set (${PROJECT_NAME}_source_files 
	Plugin.h
	PluginFramework.h
	PluginInterface.h
	PluginManager.cpp
	PluginManager.h
	SharedLibrary.cpp
	SharedLibrary.h
)

add_library (${PROJECT_NAME} ${${PROJECT_NAME}_source_files})
