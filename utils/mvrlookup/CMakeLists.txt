
project(MVRLookup)

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})

set(source_files
  MVRLookup.cpp
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(Threads)
	find_package(X11)
	set(ALL_LIBS ${ALL_LIBS} ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xi m dl ${X11_LIBRARIES})
endif()


add_executable(${PROJECT_NAME} ${source_files} ${config_files})

target_link_libraries(${PROJECT_NAME} MinVR ${OPENGL_gl_LIBRARY} ${ALL_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "utils")

install(TARGETS MVRLookup RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
