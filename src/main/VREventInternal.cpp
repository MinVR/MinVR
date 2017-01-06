/** 
This file is part of the MinVR Open Source Project, which is developed and 
maintained collaboratively by the University of Minnesota and Brown University.

Copyright (c) 2016 Regents of the University of Minnesota and Brown University.
This software is distributed under the BSD-3 Clause license, which can be found
at: MinVR/LICENSE.txt.

Original Author(s) of this File: 
	Dan Keefe, 2016, University of Minnesota
	
Author(s) of Significant Updates/Modifications to the File:
	...
*/

#include "VREventInternal.h"
#include <api/VREvent.h>
#include <config/VRCoreTypes.h>


namespace MinVR {

VREventInternal::VREventInternal(const std::string &name, VRDataIndex *dataIndex) :
  _name(name), _event(this), _dataIndex(dataIndex)
{
  // Copy event data from the data index into local storage and C-style arrays
  std::list<std::string> fields = _dataIndex->getNames();
  for (std::list<std::string>::iterator field = fields.begin(); field != fields.end(); ++field) {
    VRCORETYPE_ID t = _dataIndex->getType(*field, getName());
    if (t == VRCORETYPE_INT) {
      _intMap[*field] = _dataIndex->getValue(*field, _name);
    }
    else if (t == VRCORETYPE_DOUBLE) {
      _doubleMap[*field] = _dataIndex->getValue(*field, _name);
    }
    else if (t == VRCORETYPE_STRING) {
      std::string s = _dataIndex->getValue(*field, _name);
      char *c = new char[s.size()+1];
      strncpy(c, s.c_str(), s.size());
      c[s.size()] = '\0';
      _charArrayMap[*field] = c;
      _charArraySizes[*field] = s.size();
    }
    else if (t == VRCORETYPE_INTARRAY) {
      std::vector<int> v = _dataIndex->getValue(*field, _name);
      int *a = new int[v.size()];
      std::copy(v.begin(), v.end(), a);
      _intArrayMap[*field] = a;
      _intArraySizes[*field] = v.size();
    }
    else if (t == VRCORETYPE_DOUBLEARRAY) {
      std::vector<double> v = _dataIndex->getValue(*field, _name);
      double *a = new double[v.size()];
      std::copy(v.begin(), v.end(), a);
      _doubleArrayMap[*field] = a;
      _doubleArraySizes[*field] = v.size();
    }
    else {
      // Error here:  not sure what to do with other data index types
    }
  }
}

  
VREventInternal::~VREventInternal() {
  // Clean up memory for data copied over into C-style arrays
  for (std::unordered_map<std::string, char*>::iterator it = _charArrayMap.begin(); it != _charArrayMap.end(); ++it) {
    delete [] it->second;
  }
  for (std::unordered_map<std::string, int*>::iterator it = _intArrayMap.begin(); it != _intArrayMap.end(); ++it) {
    delete [] it->second;
  }
  for (std::unordered_map<std::string, double*>::iterator it = _doubleArrayMap.begin(); it != _doubleArrayMap.end(); ++it) {
    delete [] it->second;
  }
}
  

int VREventInternal::getDataAsInt(const std::string &fieldName) const {
  return _intMap.at(fieldName);
}

float VREventInternal::getDataAsDouble(const std::string &fieldName) const {
  return _doubleMap.at(fieldName);
}

const char * VREventInternal::getDataAsCharArray(const std::string &fieldName) const {
  return _charArrayMap.at(fieldName);
}

int VREventInternal::getCharArraySize(const std::string &fieldName) const {
  return _charArraySizes.at(fieldName);
}
  
const int * VREventInternal::getDataAsIntArray(const std::string &fieldName) const {
  return _intArrayMap.at(fieldName);
}

int VREventInternal::getIntArraySize(const std::string &fieldName) const {
  return _intArraySizes.at(fieldName);
}

const double * VREventInternal::getDataAsDoubleArray(const std::string &fieldName) const {
  return _doubleArrayMap.at(fieldName);
}
  
int VREventInternal::getDoubleArraySize(const std::string &fieldName) const {
  return _doubleArraySizes.at(fieldName);
}

VREvent::DataType VREventInternal::getDataType(const std::string &fieldName) const {
  if (_intMap.find(fieldName) != _intMap.end()) {
    return VREvent::DataType::IntData;
  }
  else if (_doubleMap.find(fieldName) != _doubleMap.end()) {
    return VREvent::DataType::DoubleData;
  }
  else if (_charArrayMap.find(fieldName) != _charArrayMap.end()) {
    return VREvent::DataType::CharArrayData;
  }
  else if (_intArrayMap.find(fieldName) != _intArrayMap.end()) {
    return VREvent::DataType::IntArrayData;
  }
  else if (_doubleArrayMap.find(fieldName) != _doubleArrayMap.end()) {
    return VREvent::DataType::DoubleArrayData;
  }
  // TODO: Error, throw exception???
  return VREvent::DataType::IntData;
}
  
  
VREvent* VREventInternal::getAPIEvent() {
	return &_event;
}

VRDataIndex* VREventInternal::getDataIndex() {
	return _dataIndex;
}


}
