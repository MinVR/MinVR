project(ConfigExample)



set(vr_exercz_source_files
  exercz.cpp
)

add_executable(exercz ${vr_exercz_source_files})

target_link_libraries(exercz MinVR ${ALL_LIBS})

set_property(TARGET exercz PROPERTY FOLDER "examples")



set(vr_sample_source_files
  sample.cpp
)

add_executable(sample ${vr_sample_source_files})

target_link_libraries(sample MinVR ${ALL_LIBS})

set_property(TARGET sample PROPERTY FOLDER "examples")
