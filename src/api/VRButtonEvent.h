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

#ifndef VRButtonEvent_H
#define VRButtonEvent_H


#include <iostream>

namespace MinVR {

class VRDataIndex;

/** 
*/
class VRButtonEvent {
public:
	VRButtonEvent(const VRDataIndex &internalIndex);
	virtual ~VRButtonEvent();


    // A string describing the name of this button
    std::string getName() const;

    // true if the button is currently in a "pressed down" state
    bool isDown() const;
    
    

	/** For advanced use, provides access to the VRDataIndex used internally by
	    MinVR, which may contain some additional data fields beyond those
	    accessible via this API. 
	 */
	const VRDataIndex& index() const;

    
    static VRDataIndex createValidDataIndex(const std::string &eventName, int buttonState);
    
private:

	const VRDataIndex &_index;
};


} // end namespace


#endif
