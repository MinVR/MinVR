project(MinVR_FreeGLUT)

set (SOURCEFILES 
  src/VRFreeGLUTInputDevice.cpp
  src/VRFreeGLUTPlugin.cpp
  src/VRFreeGLUTWindowToolkit.cpp
)

set (HEADERFILES
  src/VRFreeGLUTInputDevice.h
  src/VRFreeGLUTWindowToolkit.h
)

source_group("Header Files" FILES ${HEADERFILES})
 
#set_property(TARGET FreeGLUT PROPERTY FOLDER "Dependencies")
SET(CMAKE_MODULE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH} )
find_package(FreeGLUT REQUIRED)

message(STATUS "FreeGLUT libraries: " ${FREEGLUT_LIBRARIES})
message(STATUS "FreeGLUT includes:  " ${FREEGLUT_INCLUDE_DIRS})

include_directories(${FREEGLUT_INCLUDE_DIR})

# OpenGL
find_package(OpenGL REQUIRED)
set(OPENGL_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
set(OPENGL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIRS})


#------------------------------------------
# MinVR Dependencies
#------------------------------------------

link_directories(${CMAKE_BINARY_DIR}/lib)

#------------------------------------------
# Include Directories
#------------------------------------------

# Include Directories
include_directories (
  ${vr_src_dir}
  src
  deps/FreeGLUT/include
)

#------------------------------------------
# Output Directories
#------------------------------------------

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/bin)
foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
	string (TOUPPER ${CONF} CONF)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/bin)
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF} ${CMAKE_BINARY_DIR}/plugins/${PROJECT_NAME}/lib)
endforeach(CONF CMAKE_CONFIGURATION_TYPES)


#------------------------------------------
# Build Target
#------------------------------------------

#add_definitions(-DPLUGINPATH="${CMAKE_BINARY_DIR}/plugins")
#add_executable ( test_FreeGLUT test/test.cpp )
#target_link_libraries(test_FreeGLUT ${OPENGL_LIBRARIES} MinVR ${LIBS_ALL})

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library ( ${PROJECT_NAME} SHARED ${HEADERFILES} ${SOURCEFILES} )
target_link_libraries(${PROJECT_NAME} ${FREEGLUT_LIBRARIES} ${OPENGL_LIBRARIES} MinVR ${LIBS_ALL})

#------------------------------------------
# Install Target
#------------------------------------------

install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/lib
         ARCHIVE DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/lib
         RUNTIME DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/bin)

set_property(TARGET MinVR_FreeGLUT PROPERTY FOLDER "plugins")
