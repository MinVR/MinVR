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

#ifndef VRCursorEvent_H
#define VRCursorEvent_H


#include <iostream>
#include <vector>


namespace MinVR {

class VRDataIndex;

/** Reports state information for cursors, such as a mouse cursor or a 
    touch cursor in multi-touch environments.
*/
class VRCursorEvent {
public:
	VRCursorEvent(const VRDataIndex &internalIndex);
	virtual ~VRCursorEvent();

    
    // A string describing the name of this cursor
    std::string getName() const;
    
    // Returns the current 2D position of the cursor in the native (x,y)
    // coordinate system reported by the input device.
    const float * getPos() const;
    
    // Returns the current 2D position of the cursor normalized to fall within
    // a 0.0 to 1.0 range.
    const float * getNormalizedPos() const;
    

    // Possible:  const float * getVel();

    
    
    
    
	/** For advanced use, provides access to the VRDataIndex used internally by
	    MinVR, which may contain some additional data fields beyond those
	    accessible via this API. 
	 */
	const VRDataIndex& index() const;

    
    
    static VRDataIndex createValidDataIndex(const std::string &eventName,
        std::vector<float> position, std::vector<float> normalizedPosition);

    
private:

	const VRDataIndex &_index;
};


} // end namespace


#endif
