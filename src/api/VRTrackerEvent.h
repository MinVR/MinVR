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

#ifndef VRTrackerEvent_H
#define VRTrackerEvent_H

#include <iostream>
#include <vector>


namespace MinVR {

class VRDataIndex;

/** 
*/
class VRTrackerEvent {
public:
	VRTrackerEvent(const VRDataIndex &internalIndex);
	virtual ~VRTrackerEvent();


    // A string describing the name of this cursor
    std::string getName() const;
    
    // Returns the current position and orientation of the tracker encoded in
    // a column-major 4x4 tranformation matrix.  The coordinate system is the
    // "room coordinate space" defined via the current MinVR config files.
    const float * getTransform() const;

    
    // Returns just the positional component of the current transform in a
    // three-element array.
    const float * getPos() const;

    
    // Possible:  const float * getRotationMat();
    // Possible:  const float * getQuaternion();
    // Possible:  const float * getDeltaTransform();
    // Possible:  const float * getHeading();

    
    
    
	/** For advanced use, provides access to the VRDataIndex used internally by
	    MinVR, which may contain some additional data fields beyond those
	    accessible via this API. 
	 */
	const VRDataIndex& index() const;
    
    
    static VRDataIndex createValidDataIndex(const std::string &eventName,
                                            std::vector<float> transform);

private:

	const VRDataIndex &_index;
};


} // end namespace


#endif
