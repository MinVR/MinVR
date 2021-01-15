# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

# Builds itest-opengl-shaderpipeline-with-api.

# Note: This CMakeLists.txt is not intended to work on its own.  It inherits many settings 
# from the root CMakeLists.txt (build and install directories, compiling flags, #defines, etc.)


# By convention, interactive tests begin with "itest-"
project(vtk-with-api)


# Source:
set (SOURCEFILES 
  main.cpp
  
)
set (HEADERFILES
  
)
set (CONFIGFILES
)

find_path(MINVR_VTK_INCLUDE REQUIRED
	NAMES
	VRVTKApp.h
	PATH_SUFFIXES
	include
	HINTS
    ${CMAKE_INSTALL_PREFIX}/include/MinVR-2.0/MinVR_VTK
	ENV CPATH # For OSCAR modules at Brown/CCV
	)
	
find_library(MINVR_VTK_LIB REQUIRED
	NAMES
	MinVR_VTK
	MinVR_VTKd
	PATH_SUFFIXES
	${_libsuffixes}
	HINTS
    ${CMAKE_INSTALL_PREFIX}/lib/MinVR-2.0
	ENV LIBRARY_PATH # For OSCAR modules at Brown/CCV
	)

find_path(VTK_INCLUDE REQUIRED
	NAMES
	ExternalVTKWidget.h
	vtkExternalOpenGLCamera.h
	HINTS
	"${VTK_INCLUDE_DIRS}"
	ENV CPATH # For OSCAR modules at Brown/CCV
	)

#gainput name must be out of the path


find_library(VTK_LIBRARY REQUIRED
	NAMES
	vtkCommonCore
	vtkCommonExecutionModel
	vtkRenderingCore
	vtkFiltersSources
	HINTS
    ${VTK_LIBRARIES}
	ENV LIBRARY_PATH # For OSCAR modules at Brown/CCV
	)

# Define the target
add_executable(${PROJECT_NAME} ${HEADERFILES} ${SOURCEFILES})


# Setup dependency on libMinVR:
target_include_directories(${PROJECT_NAME} PUBLIC ../../src
                                           PUBLIC ${MINVR_VTK_INCLUDE}
                                           PUBLIC ${VTK_INCLUDE})



target_link_libraries(${PROJECT_NAME} PUBLIC MinVR)


target_link_libraries(${PROJECT_NAME} 
                     PUBLIC debug  ${MINVR_VTK_LIB}
)

target_link_libraries(${PROJECT_NAME} 
                     PUBLIC optimized  ${MINVR_VTK_LIB}
)
		
target_link_libraries(${PROJECT_NAME} 
                     PUBLIC debug  ${VTK_LIBRARY}/vtkCommonCored.lib
					 PUBLIC debug ${VTK_LIBRARY}/vtkRenderingCored.lib
                     PUBLIC debug  ${VTK_LIBRARY}/vtkFiltersSourcesd.lib
					 PUBLIC debug  ${VTK_LIBRARY}/vtkCommonExecutionModeld.lib
)
		
target_link_libraries(${PROJECT_NAME} 
            PUBLIC optimized ${VTK_LIBRARY}/vtkCommonCore.lib 
			PUBLIC optimized ${VTK_LIBRARY}/vtkRenderingCore.lib
			PUBLIC debug  ${VTK_LIBRARY}/vtkFiltersSourcesd.lib
			PUBLIC debug  ${VTK_LIBRARY}/vtkCommonExecutionModeld.lib
)

# Setup dependency on OpenGL:
include(AutoBuildOpenGL)
AutoBuild_use_package_OpenGL(${PROJECT_NAME} PUBLIC)


# Setup dependency on GLEW (not needed on OSX):
if (NOT APPLE)
  include(AutoBuildGLEW)
  AutoBuild_use_package_GLEW(${PROJECT_NAME} PUBLIC)
endif()


# Installation:
install( TARGETS ${PROJECT_NAME}
         RUNTIME DESTINATION ${INSTALL_BIN_DEST}
         COMPONENT ITests)



# For better organization when using an IDE with folder structures:
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-interactive")
source_group("Header Files" FILES ${HEADERFILES})
set_source_files_properties(${CONFIGFILES} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Config Files" FILES ${CONFIGFILES})


