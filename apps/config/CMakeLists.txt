project(test_config)

add_executable(exercz exercz.cpp)
target_link_libraries(exercz VRIndex)

add_executable(sample sample.cpp)
target_link_libraries(sample VRIndex)

