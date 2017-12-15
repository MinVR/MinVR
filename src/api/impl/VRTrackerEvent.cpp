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


#include "api/VRTrackerEvent.h"
#include "config/VRDataIndex.h"

namespace MinVR {

VRTrackerEvent::VRTrackerEvent(const VRDataIndex &internalIndex) : _index(internalIndex) {

}

VRTrackerEvent::~VRTrackerEvent() {

}

    
const float * VRTrackerEvent::getTransform() const {
    if (_index.exists("Transform")) {
        const std::vector<float> *v = _index.getValue("Transform");
        return &(v->front());
    }
    else {
        VRERROR("VRTrackerEvent::getTransform() cannot determine a data field to return for event named " +
                _index.getName() + ".", "Tracker_Move events should have an entry in their data index called Transform.");
        return NULL;
    }
}


const float * VRTrackerEvent::getPos() const {
    return &getTransform()[12];
}
    
    
std::string VRTrackerEvent::getName() const {
    return _index.getName();
}

    
const VRDataIndex& VRTrackerEvent::index() const {
	return _index;
}


VRDataIndex VRTrackerEvent::createValidDataIndex(const std::string &eventName,
                                                 std::vector<float> transform)
{
    VRDataIndex di(eventName);
    di.addData("EventType", "TrackerMove");
    di.addData("Transform", transform);
    return di;
}
    

} // end namespace
