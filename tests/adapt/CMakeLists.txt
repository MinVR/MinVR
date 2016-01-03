cmake_minimum_required (VERSION 2.6)

project (adapt)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(
  /usr/local/include
  /usr/local/include/GLFW
  /usr/local/X11/include
  /opt/X11/include
  )

set(ALL_LIBS
	${OPENGL_LIBRARY}
  ${GLUT_LIBRARY}
  )

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
  # This refers to gluLookAt and gluPerspective, at least under OS X
  -Wno-deprecated-declarations
  )

add_executable (adaptee
  adaptee.cpp
  )

target_link_libraries (adaptee ${ALL_LIBS})

add_executable (adapted
  adapted.cpp
  )

target_link_libraries (adapted VRMain VRIndex VRMath VRNet ${ALL_LIBS})


