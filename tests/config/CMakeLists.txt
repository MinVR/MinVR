# Create a library from the source files in this directory.

project (memtest)

add_executable (memtest memtest.cpp)
target_link_libraries (memtest VRIndex)

