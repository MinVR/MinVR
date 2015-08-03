# cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..
cmake_minimum_required(VERSION 2.8)
project("MinVRSettings")


add_executable(settings_driver main.cpp
                               MinVRSettings.cpp
			       attribute.cpp
                               Cxml.cpp
                               element.cpp
                               utils.cpp
                               XML.cpp)

add_executable(xml_driver driver.cpp
                          attribute.cpp
                          Cxml.cpp
                          element.cpp
                          utils.cpp
                          XML.cpp)
