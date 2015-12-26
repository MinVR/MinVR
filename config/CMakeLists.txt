cmake_minimum_required(VERSION 2.8)
project(VRIndex)

set(${PROJECT_NAME}_headers
  VRDatum.h
  VRDatumFactory.h
  VRDataIndex.h
  VRDataQueue.h
  )

set(${PROJECT_NAME}_source_files
  ${${PROJECT_NAME}_headers}
  VRDatum.cpp
  VRDatumFactory.cpp
  VRDataIndex.cpp
  VRDataQueue.cpp
  Cxml/attribute.cpp
  Cxml/Cxml.cpp
  Cxml/element.cpp
  Cxml/utils.cpp
  Cxml/XML.cpp
  )

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_source_files})

install(DIRECTORY ${PROJECT_SOURCE_DIR}  DESTINATION "include" FILES_MATCHING PATTERN "*.h")

install( TARGETS ${PROJECT_NAME}
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         RUNTIME DESTINATION bin)