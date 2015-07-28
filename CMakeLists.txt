project("MinVRSettings")


add_executable(settings_driver main.cpp
                               MinVRSettings.cpp)

add_executable(xml_driver driver.cpp
                          attribute.cpp
                          Cxml.cpp
                          element.cpp
                          utils.cpp
                          XML.cpp)
