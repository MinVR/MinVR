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


#include "api/VRCursorEvent.h"
#include "config/VRDataIndex.h"

namespace MinVR {

VRCursorEvent::VRCursorEvent(const VRDataIndex &internalIndex) : _index(internalIndex) {

}

VRCursorEvent::~VRCursorEvent() {

}
    
const float * VRCursorEvent::getPos() const {
    if (_index.exists("Position")) {
        const std::vector<float>* v = _index.getValue("Position");
        return &(v->front());
    }
    else {
        VRERROR("VRCursorEvent::getPos() cannot determine a data field to return for event named " +
                _index.getName() + ".", "Cursor events should have an entry in their data index called Position.");
        return NULL;
    }
}
    
const float * VRCursorEvent::getNormalizedPos() const {
    if (_index.exists("NormalizedPosition")) {
        const std::vector<float>* v = _index.getValue("NormalizedPosition");
        return &(v->front());
    }
    else {
        VRERROR("VRCursorEvent::getNormalizedPos() cannot determine a data field to return for event named " +
                _index.getName() + ".", "Cursor events should have an entry in their data index called NormalizedPosition.");
        return NULL;
    }
    
}
    
    

std::string VRCursorEvent::getName() const {
    return _index.getName();
}

    
    
const VRDataIndex& VRCursorEvent::index() const {
	return _index;
}

    
    
VRDataIndex VRCursorEvent::createValidDataIndex(const std::string &eventName,
    std::vector<float> position, std::vector<float> normalizedPosition)
{
    VRDataIndex di(eventName);
    di.addData("EventType", "CursorMove");
    di.addData("Position", position);
    di.addData("NormalizedPosition", normalizedPosition);
    return di;
}
    

} // end namespace
