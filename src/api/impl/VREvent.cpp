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

#include <iostream>

namespace MinVR {


  
// This is the only constructor that needs to be implemented. The default and 
// copy constructors are listed as private simply to avoid accidental use.
VREvent::VREvent(VREventInternal *internalEvent) : _internal(internalEvent) {

}


std::string VREvent::getName() const {
	return _internal->getName();
}

std::vector<std::string> VREvent::getDataFields() const {
  return _internal->getDataFields();
}

int VREvent::getDataAsInt(const std::string &fieldName) const {
  return _internal->getDataAsInt(fieldName);
}

float VREvent::getDataAsFloat(const std::string &fieldName) const {
  return _internal->getDataAsFloat(fieldName);
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

const float * VREvent::getDataAsFloatArray(const std::string &fieldName) const {
  return _internal->getDataAsFloatArray(fieldName);
}

int VREvent::getFloatArraySize(const std::string &fieldName) const {
  return _internal->getFloatArraySize(fieldName);
}
  
VREvent::DataType VREvent::getDataType(const std::string &fieldName) const {
  return _internal->getDataType(fieldName);
}

void VREvent::print() const {
  std::cout << "VREvent '" << getName() << "' with ";
  int n = getDataFields().size();
  if (n == 0) {
    std::cout << "no data fields." << std::endl;
  }
  else {
    std::cout << n << " data fields:" << std::endl;
    std::vector<std::string> fields = getDataFields();
    for (std::vector<std::string>::iterator field = fields.begin(); field != fields.end(); ++field) {
      DataType t = getDataType(*field);
      if (t == IntData) {
        std::cout << "  " << *field << " (int)           = " << getDataAsInt(*field) << std::endl;
      }
      else if (t == FloatData) {
        std::cout << "  " << *field << " (float)        = " << getDataAsFloat(*field) << std::endl;
      }
      else if (t == CharArrayData) {
        std::cout << "  " << *field << " (char array)    = " << getDataAsCharArray(*field) << std::endl;
      }
      else if (t == IntArrayData) {
        std::cout << "  " << *field << " (int array)     = [";
        for (int i=0;i<getIntArraySize(*field);i++) {
          if (i>0) {
            std::cout << ", ";
          }
          std::cout << getDataAsIntArray(*field)[i];
        }
        std::cout << "]" << endl;
      }
      else if (t == FloatArrayData) {
        std::cout << "  " << *field << " (float array)  = [";
        for (int i=0;i<getFloatArraySize(*field);i++) {
          if (i>0) {
            std::cout << ", ";
          }
          std::cout << getDataAsFloatArray(*field)[i];
        }
        std::cout << "]" << endl;
      }
    }
  }
}
  
  

VREventInternal* VREvent::getInternal() const {
	return _internal;
}



} // end namespace
