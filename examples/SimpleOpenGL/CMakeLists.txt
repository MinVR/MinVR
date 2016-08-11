
project(SimpleOpenGL)

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})

set(vr_example_source_files
  vrexample.cpp
)

set(vr_example_config_files
  3DTV.xml
  desktop.xml
  ivlabcave.xml
  ivlabcave_multithreaded.xml
  ivlabcavefront.xml
  simple.xml
  testalldisplaynodes.xml
  multithreaded.xml
  YURT.xml
)

set_source_files_properties(${vr_example_config_files} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group("Config Files" FILES ${vr_example_config_files})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(Threads)
	find_package(X11)
	set(ALL_LIBS ${ALL_LIBS} ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xi m dl ${X11_LIBRARIES})
endif()


add_executable(SimpleOpenGL ${vr_example_source_files} ${vr_example_config_files})

target_link_libraries(SimpleOpenGL MinVR ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${ALL_LIBS})

set_property(TARGET SimpleOpenGL PROPERTY FOLDER "examples")

