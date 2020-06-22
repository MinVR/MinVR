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

#ifndef VRWindowResizeEvent_H
#define VRWindowResizeEvent_H


#include <iostream>
#include <vector>


namespace MinVR {

class VRDataIndex;

/** Reports state information for cursors, such as a mouse cursor or a 
    touch cursor in multi-touch environments.
*/
class VRWindowResizeEvent {
public:
  VRWindowResizeEvent(const VRDataIndex &internalIndex);
	virtual ~VRWindowResizeEvent();

    
    // A string describing the name of this cursor
    std::string getName() const;
    
    
    // returns the new window's size (w,h)
    // 
    const float * getWindowSize () const;
    

    // Possible:  const float * getVel();

    
    
    
    
	/** For advanced use, provides access to the VRDataIndex used internally by
	    MinVR, which may contain some additional data fields beyond those
	    accessible via this API. 
	 */
	const VRDataIndex& index() const;

    
    
    static VRDataIndex createValidDataIndex(const std::string &eventName,
        std::vector<float> position);

    
private:

	const VRDataIndex &_index;
};


} // end namespace


#endif
