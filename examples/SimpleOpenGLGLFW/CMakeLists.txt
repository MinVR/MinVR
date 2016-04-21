
project(SimpleOpenGLGLFW)

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})

add_executable(SimpleOpenGLGLFW vrexample.cpp)

target_link_libraries(SimpleOpenGLGLFW MinVR ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})

