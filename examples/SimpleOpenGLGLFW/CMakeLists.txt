
project(SimpleOpenGLGLFW)

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})

set(vr_example_source_files
  vrexample.cpp
)

set(vr_example_config_files
  desktop.xml
  ivlabcave.xml
  simple.xml
  YURT.xml
)

set_source_files_properties(${vr_example_config_files} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group("Config Files" FILES ${vr_example_config_files})



add_executable(SimpleOpenGLGLFW ${vr_example_source_files} ${vr_example_config_files})

target_link_libraries(SimpleOpenGLGLFW MinVR ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})

set_property(TARGET SimpleOpenGLGLFW PROPERTY FOLDER "examples")

