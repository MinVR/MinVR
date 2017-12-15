# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

project(itest-config-viewer)



set(vr_exercz_source_files
  exercz.cpp
)

add_executable(${PROJECT_NAME} ${vr_exercz_source_files})

target_link_libraries(${PROJECT_NAME} MinVR ${ALL_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tests-interactive")



# TODO: move this over to tests-batch
#set(vr_sample_source_files
#  sample.cpp
#)
#add_executable(sample-config-program ${vr_sample_source_files})
#target_link_libraries(sample-config-program MinVR ${ALL_LIBS})
#set_property(TARGET sample-config-program PROPERTY FOLDER "tests-batch")
