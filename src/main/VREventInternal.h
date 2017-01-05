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

#ifndef VREVENTINTERNAL_H_
#define VREVENTINTERNAL_H_


#include <api/VREvent.h>
#include <config/VRDataIndex.h>


namespace MinVR {

/** Provides access to secondary and raw event data that go beyond that 
    available via the VREvent interface.  VREvent is intended to be 
    sufficient for 90% of use cases; the VREventInternal class is useful
    for the 10% of users who require low-level access for research projects
    and other advanced cases.
*/
class VREventInternal {
public:

	/** Constructs a VREventInternal and a corresponding VREvent based on the event name and data provided.
	*/
	VREventInternal(const std::string &name, VRDataIndex *dataIndex);

    /** Returns the name of the event, not to be confused with the event's
        data payload.
     */
    std::string getName() { return _name; }
  

    /** Returns a pointer to the streamlined VREvent data type associated with this VREventInternal.
    */
    VREvent* getAPIEvent();

    /** Returns a pointer to the input device that generated this event.
    */
    //VRInputDevice *getInputDevice();


    /** Returns a pointer to the underlying raw data index used to implement the VREventInternal. 
    */
	VRDataIndex* getDataIndex();

private:

    std::string  _name;
	VREvent      _event;
	VRDataIndex* _dataIndex;

};


} // end namespace

#endif
