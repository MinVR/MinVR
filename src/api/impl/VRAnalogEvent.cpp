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


#include "api/VRAnalogEvent.h"
#include "config/VRDataIndex.h"

namespace MinVR {

VRAnalogEvent::VRAnalogEvent(const VRDataIndex &internalIndex) : _index(internalIndex) {

}

VRAnalogEvent::~VRAnalogEvent() {

}

std::string VRAnalogEvent::getName() const {
    return _index.getName();
}

float VRAnalogEvent::getValue() const {
    if (_index.exists("AnalogValue")) {
        return _index.getValue("AnalogValue");
    }
    else {
        VRERROR("VRAnalogEvent::getValue() cannot determine a data field to return for event named " +
                _index.getName() + ".", "Analog events should have an entry in their data index called AnalogValue.");
        return 0.0f;
    }
}




const VRDataIndex& VRAnalogEvent::index() const {
	return _index;
}



VRDataIndex VRAnalogEvent::createValidDataIndex(const std::string &eventName, float analogValue) {
    VRDataIndex di(eventName);
    di.addData("EventType", "AnalogUpdate");
    di.addData("AnalogValue", analogValue);
    return di;
}


} // end namespace
