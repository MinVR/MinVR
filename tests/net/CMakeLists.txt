# Create a library from the source files in this directory.

project (test_net)

add_executable (test_netclient client.cpp)
target_link_libraries (test_netclient VRNet)

add_executable (test_netserver server.cpp)
target_link_libraries (test_netserver VRNet)

