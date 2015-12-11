cmake_minimum_required(VERSION 2.8)
project(VRIndex)

set(${PROJECT_NAME}_headers
  VRDatum.h
  VRDatumFactory.h
  VRDataCollection.h
  VRDataIndex.h
  )

set(${PROJECT_NAME}_source_files
  ${${PROJECT_NAME}_headers}
  VRDatum.cpp
  VRDatumFactory.cpp
  VRDataCollection.cpp
  VRDataIndex.cpp
  Cxml/attribute.cpp
  Cxml/Cxml.cpp
  Cxml/element.cpp
  Cxml/utils.cpp
  Cxml/XML.cpp
  )

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_source_files})

add_executable(exercz exercz.cpp ${${PROJECT_NAME}_headers})
target_link_libraries(exercz ${PROJECT_NAME})

add_executable(sample sample.cpp ${${PROJECT_NAME}_headers})
target_link_libraries(sample ${PROJECT_NAME})

