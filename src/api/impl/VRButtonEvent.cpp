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


#include "api/VRButtonEvent.h"
#include "config/VRDataIndex.h"

namespace MinVR {

VRButtonEvent::VRButtonEvent(const VRDataIndex &internalIndex) : _index(internalIndex) {

}

VRButtonEvent::~VRButtonEvent() {

}

    
bool VRButtonEvent::isDown() const {
    if (_index.exists("ButtonState")) {
        std::vector<float> v = _index.getValue("ButtonState");
        return &v.front();
    }
    else {
        VRERROR("VRButtonEvent::isDown() cannot determine a data field to return for event named " +
                _index.getName() + ".", "Button events should have an entry in their data index called ButtonState that stores a 0=up or 1=down.");
        return NULL;
    }
}
    
    
std::string VRButtonEvent::getName() const {
    return _index.getName();
}

    
const VRDataIndex& VRButtonEvent::index() const {
	return _index;
}

    
    
VRDataIndex VRButtonEvent::createValidDataIndex(const std::string &eventName, int buttonState) {
    VRDataIndex di(eventName);
    if (buttonState) {
        di.addData("EventType", "ButtonDown");
    }
    else {
        di.addData("EventType", "ButtonUp");
    }
    di.addData("ButtonState", buttonState);
    return di;
}
    

} // end namespace
