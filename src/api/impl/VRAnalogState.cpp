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


#include "api/VRAnalogState.h"
#include "config/VRDataIndex.h"

namespace MinVR {

VRAnalogState::VRAnalogState(const VRDataIndex &internalIndex) : _index(internalIndex) {

}

VRAnalogState::~VRAnalogState() {

}

std::string VRAnalogState::getName() const {
    return _index.getName();
}
    
float VRAnalogState::getValue() const {
    if (_index.exists("DefaultData")) {
        return _index.getValue("DefaultData");
    }
    else {
        std::list<std::string> keys = _index.findAllNames();
        if (keys.size() == 1) {
            return _index.getValue(*keys.begin());
        }
        else {
            VRERROR("VRAnalogState::getValue() cannot determine a default data field to return for event named " +
                    _index.getName() + ", which contains more than one data field.",
                    "Analog events with more than one data field should have an entry in their data index called "
                    "DefaultData to identify the primary data field to return when getValue() is called.");
            return 0.0;
        }
    }
}
    
    

    
const VRDataIndex& VRAnalogState::index() const {
	return _index;
}


} // end namespace
