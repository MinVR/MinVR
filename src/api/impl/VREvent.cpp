/** 
This file is part of the MinVR Open Source Project, which is developed and 
maintained collaboratively by the University of Minnesota and Brown University.

Copyright (c) 2016 Regents of the University of Minnesota and Brown University.
This software is distributed under the BSD-3 Clause license, which can be found
at: MinVR/LICENSE.txt.

Original Author(s) of this File: 
	Dan Keefe, 2017, University of Minnesota

Author(s) of Significant Updates/Modifications to the File:
	...
*/

#include "../VREvent.h"
#include <main/VREventInternal.h>
#include <config/VRDataIndex.h>
#include <config/VRCoreTypes.h>

namespace MinVR {


  
// This is the only constructor that needs to be implemented. The default and 
// copy constructors are listed as private simply to avoid accidental use.
VREvent::VREvent(VREventInternal *internalEvent) : _internal(internalEvent) {

}


std::string VREvent::getName() const {
	return _internal->getName();
}

std::list<std::string> VREvent::getDataFields() const {
	return _internal->getDataIndex()->getNames();
}

int VREvent::getDataAsInt(const std::string &fieldName) const {
  return _internal->getDataAsInt(fieldName);
}

float VREvent::getDataAsDouble(const std::string &fieldName) const {
  return _internal->getDataAsDouble(fieldName);
}

const char * VREvent::getDataAsCharArray(const std::string &fieldName) const {
  return _internal->getDataAsCharArray(fieldName);
}
  
int VREvent::getCharArraySize(const std::string &fieldName) const {
  return _internal->getCharArraySize(fieldName);
}

const int * VREvent::getDataAsIntArray(const std::string &fieldName) const {
  return _internal->getDataAsIntArray(fieldName);
}

int VREvent::getIntArraySize(const std::string &fieldName) const {
  return _internal->getIntArraySize(fieldName);
}

const double * VREvent::getDataAsDoubleArray(const std::string &fieldName) const {
  return _internal->getDataAsDoubleArray(fieldName);
}

int VREvent::getDoubleArraySize(const std::string &fieldName) const {
  return _internal->getDoubleArraySize(fieldName);
}

  
VREvent::DataType VREvent::getDataType(const std::string &fieldName) const {
  return _internal->getDataType(fieldName);
}


VREventInternal* VREvent::getInternal() const {
	return _internal;
}



} // end namespace
