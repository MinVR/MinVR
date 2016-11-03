#------------------------------------------
# Download dependencies
#------------------------------------------

set(DepName tuio)

set(${DepName}_checkout_Dir ${PROJECT_SOURCE_DIR}/lib/${DepName}/src)
make_directory(${${DepName}_checkout_Dir})
include(ExternalProject)

set(${DepName}_build_dir ${CMAKE_BINARY_DIR}/deps/${DepName})
set(${DepName}_stamp_dir ${${DepName}_build_dir}/stamp)
set(${DepName}_tmp_dir ${${DepName}_build_dir}/tmp)

set(${DepName}_PREFIX "${${DepName}_checkout_Dir}")
set(${DepName}_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/deps/${DepName}")

set(${DepName}_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${${DepName}_INSTALL_DIR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
set(${DepName}_DIR "${${DepName}_INSTALL_DIR}")

	set(${DepName}_PATCH ${CMAKE_COMMAND} -E copy ${${DepName}_checkout_Dir}/../patch/CMakeLists.txt <SOURCE_DIR>/CMakeLists.txt && ${CMAKE_COMMAND} -E copy ${${DepName}_checkout_Dir}/../patch/TuioServer.cpp <SOURCE_DIR>/TUIO/TuioServer.cpp)

	if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(${DepName}_CMAKE_ARGS ${${DepName}_CMAKE_ARGS} -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD=c++11 -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY=libc++)
	endif()

	ExternalProject_add(${DepName}
			PREFIX ${${DepName}_PREFIX}
			URL http://prdownloads.sourceforge.net/reactivision/TUIO_CPP-1.4.zip
			INSTALL_DIR ${${DepName}_INSTALL_DIR}
			CMAKE_ARGS ${${DepName}_CMAKE_ARGS}
			PATCH_COMMAND ${${DepName}_PATCH}
			UPDATE_COMMAND ""
		DOWNLOAD_DIR ${${DepName}_checkout_Dir}
		SOURCE_DIR ${${DepName}_checkout_Dir}
		TMP_DIR ${${DepName}_tmp_dir}
		STAMP_DIR ${${DepName}_stamp_dir}
		BINARY_DIR ${${DepName}_build_dir}
	 )		
 
set_property(TARGET ${DepName} PROPERTY FOLDER "Dependencies")

#install(DIRECTORY ${${DepName}_checkout_Dir}/plugin/include/ DESTINATION "${${DepName}_INSTALL_DIR}/include")
#install(DIRECTORY ${${DepName}_build_dir}/lib/  DESTINATION "${${DepName}_INSTALL_DIR}/lib/" FILES_MATCHING PATTERN "*.*")
#install(DIRECTORY ${${DepName}_build_dir}/bin/  DESTINATION "${${DepName}_INSTALL_DIR}/bin/" FILES_MATCHING PATTERN "*.*")

include_directories(${${DepName}_INCLUDE_DIR})