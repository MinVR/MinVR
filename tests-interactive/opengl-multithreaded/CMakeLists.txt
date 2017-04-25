
project(itest-opengl-multithreaded)

find_package(OpenGL REQUIRED)

include_directories(
	${OPENGL_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/third-party/GLEW
)

set(source_files
  ${CMAKE_SOURCE_DIR}/third-party/GLEW/glew.c
  main.cpp
  MyAppModel.cpp
  MyAppController.cpp
  MyAppView.cpp
  MyAppSharedContext.cpp
)

set(header_files
  MyAppModel.h
  MyAppController.h
  MyAppView.h
  MyAppSharedContext.h
)

set(config_files
)

set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Header Files" FILES ${header_files})

set_source_files_properties(${config_files} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Config Files" FILES ${config_files})

#------------------------------------------
# Specific preprocessor defines
#------------------------------------------

# Windows Section #
if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${WINDOWS_BINARIES} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	find_library(COCOA_LIB Cocoa)
	find_library(IOKIT_LIB IOKit)
	find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
    	find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
	set(LIBS_ALL ${LIBS_ALL} ${COCOA_LIB} ${IOKIT_LIB} ${CORE_FOUNDATION_FRAMEWORK} ${CORE_VIDEO_FRAMEWORK})
	message(STATUS "${CORE_VIDEO_FRAMEWORK}")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif()
  find_package(Threads)
  find_package(X11)
  set(ALL_LIBS ${ALL_LIBS} ${CMAKE_THREAD_LIBS_INIT} rt Xrandr Xxf86vm Xi m dl ${X11_LIBRARIES})
endif()

#------------------------------------------
# Build Application
#------------------------------------------

add_executable(${PROJECT_NAME} ${source_files} ${config_files} ${header_files})

target_link_libraries(${PROJECT_NAME} MinVR ${OPENGL_gl_LIBRARY} ${ALL_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-interactive")

