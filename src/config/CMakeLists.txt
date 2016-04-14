
set(${PROJECT_NAME}_source_files
  ${${PROJECT_NAME}_source_files}
  ${CMAKE_CURRENT_SOURCE_DIR}/VRDatum.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VRDatumFactory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VRDataIndex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VRDataQueue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Cxml/attribute.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Cxml/Cxml.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Cxml/element.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Cxml/utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Cxml/XML.cpp
  PARENT_SCOPE
  )
