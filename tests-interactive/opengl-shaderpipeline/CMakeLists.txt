
project(itest-opengl-shaderpipeline)

find_package(OpenGL REQUIRED)

include_directories(
	${OPENGL_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/third-party/GLEW
)

set(source_files
  ${CMAKE_SOURCE_DIR}/third-party/GLEW/glew.c
  main.cpp
)

set_source_files_properties(${config_files} PROPERTIES HEADER_FILE_ONLY TRUE)


source_group("Config Files" FILES ${config_files})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(Threads)
	find_package(X11)
	set(ALL_LIBS ${ALL_LIBS} ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xi m dl ${X11_LIBRARIES})
endif()

include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(${PROJECT_NAME} MinVR ${OPENGL_gl_LIBRARY} ${ALL_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-interactive")

