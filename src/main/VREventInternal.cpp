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
        _name(name), _indexName(name), _event(this), _dataIndex(dataIndex) {
    
    // Remove the leading '/' that comes in from the DataIndex -- users will not
    // expend an event name to begin with a '/'
    if ((_name[0] == '/') && (_name.length() >= 2)) {
        _name = _name.substr(1);
    }
            
    // The event name should always be a VRContainer (i.e., std::list<std::string>)
    // inside the DataIndex.  So, this getValue will return a list of all the
    // data fields defined inside that container.
    std::list<std::string> fields = _dataIndex->getValue(_indexName);

    // Copy event data from the data index into local storage and C-style arrays
    for (std::list<std::string>::iterator field = fields.begin(); field != fields.end(); ++field) {
        VRCORETYPE_ID t = _dataIndex->getType(*field, _indexName);
        if (t == VRCORETYPE_INT) {
            _intMap[*field] = _dataIndex->getValue(*field, _indexName);
            _fieldNames.push_back(*field);
        }
        else if (t == VRCORETYPE_FLOAT) {
            _floatMap[*field] = _dataIndex->getValue(*field, _indexName);
            _fieldNames.push_back(*field);
        }
        else if (t == VRCORETYPE_STRING) {
            std::string s = _dataIndex->getValue(*field, _indexName);
            char *c = new char[s.size()+1];
            strncpy(c, s.c_str(), s.size());
            c[s.size()] = '\0';
            _charArrayMap[*field] = c;
            _charArraySizes[*field] = s.size();
            _fieldNames.push_back(*field);
        }
        else if (t == VRCORETYPE_INTARRAY) {
            std::vector<int> v = _dataIndex->getValue(*field, _indexName);
            int *a = new int[v.size()];
            std::copy(v.begin(), v.end(), a);
            _intArrayMap[*field] = a;
            _intArraySizes[*field] = v.size();
            _fieldNames.push_back(*field);
        }
        else if (t == VRCORETYPE_FLOATARRAY) {
            std::vector<float> v = _dataIndex->getValue(*field, _indexName);
            float *a = new float[v.size()];
            std::copy(v.begin(), v.end(), a);
            _floatArrayMap[*field] = a;
            _floatArraySizes[*field] = v.size();
            _fieldNames.push_back(*field);
        }
        else {
            // Error here?: not sure what to do with other data index types
        }
    }
}


VREventInternal::~VREventInternal() {
    // Clean up memory for data copied over into C-style arrays
    for (std::map<std::string, char*>::iterator it = _charArrayMap.begin(); it != _charArrayMap.end(); ++it) {
        delete [] it->second;
    }
    for (std::map<std::string, int*>::iterator it = _intArrayMap.begin(); it != _intArrayMap.end(); ++it) {
        delete [] it->second;
    }
    for (std::map<std::string, float*>::iterator it = _floatArrayMap.begin(); it != _floatArrayMap.end(); ++it) {
        delete [] it->second;
    }
}


std::vector<std::string> VREventInternal::getDataFields() const {
    return _fieldNames;
}


int VREventInternal::getDataAsInt(const std::string &fieldName) const {
    return _intMap.at(fieldName);
}

float VREventInternal::getDataAsFloat(const std::string &fieldName) const {
    return _floatMap.at(fieldName);
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

const float * VREventInternal::getDataAsFloatArray(const std::string &fieldName) const {
    return _floatArrayMap.at(fieldName);
}

int VREventInternal::getFloatArraySize(const std::string &fieldName) const {
    return _floatArraySizes.at(fieldName);
}

VREvent::DataType VREventInternal::getDataType(const std::string &fieldName) const {
    if (_intMap.find(fieldName) != _intMap.end()) {
        return VREvent::IntData;
    }
    else if (_floatMap.find(fieldName) != _floatMap.end()) {
        return VREvent::FloatData;
    }
    else if (_charArrayMap.find(fieldName) != _charArrayMap.end()) {
        return VREvent::CharArrayData;
    }
    else if (_intArrayMap.find(fieldName) != _intArrayMap.end()) {
        return VREvent::IntArrayData;
    }
    else if (_floatArrayMap.find(fieldName) != _floatArrayMap.end()) {
        return VREvent::FloatArrayData;
    }
    // TODO: Error, throw exception???
    return VREvent::IntData;
}


VREvent* VREventInternal::getAPIEvent() {
    return &_event;
}

VRDataIndex* VREventInternal::getDataIndex() {
    return _dataIndex;
}

    
}
