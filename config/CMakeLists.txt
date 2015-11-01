cmake_minimum_required(VERSION 2.8)
project("MinVR2/config")

add_executable(exercz exercz.cpp
  VRDatum.h VRDatum.cpp
  VRDatumFactory.h VRDatumFactory.cpp
  VRDataCollection.h VRDataCollection.cpp
  VRDataIndex.h VRDataIndex.cpp
  Cxml/attribute.cpp
  Cxml/Cxml.cpp
  Cxml/element.cpp
  Cxml/utils.cpp
  Cxml/XML.cpp)

add_executable(sample sample.cpp
  VRDatum.h VRDatum.cpp
  VRDatumFactory.h VRDatumFactory.cpp
  VRDataCollection.h VRDataCollection.cpp
  VRDataIndex.h VRDataIndex.cpp
  Cxml/attribute.cpp
  Cxml/Cxml.cpp
  Cxml/element.cpp
  Cxml/utils.cpp
  Cxml/XML.cpp)

