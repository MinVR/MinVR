# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.


project(MVRLookup)

find_package(OpenGL REQUIRED)

set(source_files
  MVRLookup.cpp
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(Threads)
	find_package(X11)
	set(ALL_LIBS ${ALL_LIBS} ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xi m dl ${X11_LIBRARIES})
endif()


add_executable(${PROJECT_NAME} ${source_files} ${config_files})

target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} MinVR ${OPENGL_gl_LIBRARY} ${ALL_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "utils")

install(TARGETS MVRLookup RUNTIME DESTINATION ${INSTALL_BIN_DEST} COMPONENT Utils)
