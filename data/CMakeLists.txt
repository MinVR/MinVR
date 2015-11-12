# Create a library from the source files in this directory.

project (VRData)

set (${PROJECT_NAME}_source_files 
	VRCoreTypes.h
	VRDataIndex.cpp
	VRDataIndex.h
	VRDatum.cpp
	VRDatum.h
	VRDatumFactory.cpp
	VRDatumFactory.h
	XMLUtils.cpp
	XMLUtils.h
	Cxml/attribute.cpp
    Cxml/Cxml.cpp
    Cxml/element.cpp
    Cxml/utils.cpp
    Cxml/XML.cpp
)

add_library (${PROJECT_NAME} ${${PROJECT_NAME}_source_files})
