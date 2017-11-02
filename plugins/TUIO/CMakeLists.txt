project(MinVR_TUIO)

set (SOURCEFILES 
  src/VRTUIOPlugin.cpp
  src/VRTUIODevice.cpp
)

set (HEADERFILES
  src/VRTUIODevice.h
)

source_group("Header Files" FILES ${HEADERFILES})

#------------------------------------------
# Download dependencies
#------------------------------------------

add_subdirectory(lib)

link_directories(${CMAKE_BINARY_DIR}/deps/tuio)
message(${CMAKE_BINARY_DIR}/deps/tuio)

if(MSVC)
	set(TUIO_LIBRARIES optimized TUIO.lib debug TUIOd.lib)
	set(TUIO_LIBRARIES ${TUIO_LIBRARIES} winmm.lib wininet.lib ws2_32.lib)
else()
	set(TUIO_LIBRARIES optimized libTUIO.a debug libTUIOd.a)
endif()

include_directories(
	${PROJECT_SOURCE_DIR}/lib/tuio/src/oscpack
	${PROJECT_SOURCE_DIR}/lib/tuio/src/TUIO
)

message(STATUS "TUIO includes: ${PROJECT_SOURCE_DIR}/lib/tuio/src/oscpack ${PROJECT_SOURCE_DIR}/lib/tuio/src/TUIO")
message(STATUS "TUIO libraries: ${TUIO_LIBRARIES}")

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
)

#------------------------------------------
# Specific preprocessor defines
#------------------------------------------

# Windows Section #
if (MSVC)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
endif()

#------------------------------------------
# Set output directories to lib, and bin
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
#add_executable ( test_glfw test/test.cpp )
#target_link_libraries(test_glfw ${OPENGL_LIBRARIES} MinVR ${LIBS_ALL})

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library ( ${PROJECT_NAME} SHARED ${HEADERFILES} ${SOURCEFILES} )
target_link_libraries(${PROJECT_NAME} ${TUIO_LIBRARIES} MinVR ${LIBS_ALL})
add_dependencies( ${PROJECT_NAME} tuio)

#------------------------------------------
# Install Target
#------------------------------------------

install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/lib
         ARCHIVE DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/lib
         RUNTIME DESTINATION ${VRPlugins_install_dir}/${PROJECT_NAME}/bin)

set_property(TARGET MinVR_GLFW PROPERTY FOLDER "plugins")
set_property(TARGET glfw PROPERTY FOLDER "plugins/dependencies/libGLFW")
