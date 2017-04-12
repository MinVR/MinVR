
project(itest-opengl-mvc)

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})

set(source_files
  glew.c
  main.cpp
  MyAppController.cpp
  MyAppView.cpp
)

set(header_files
  MyAppModel.h
  MyAppController.h
  MyAppView.h
)

set(config_files
  multithreaded.xml
)

set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Header Files" FILES ${header_files})

set_source_files_properties(${config_files} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Config Files" FILES ${config_files})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(Threads)
	find_package(X11)
	set(ALL_LIBS ${ALL_LIBS} ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xi m dl ${X11_LIBRARIES})
endif()


add_executable(${PROJECT_NAME} ${source_files} ${config_files} ${header_files})

target_link_libraries(${PROJECT_NAME} MinVR ${OPENGL_gl_LIBRARY} ${ALL_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-interactive")

