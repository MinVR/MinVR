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

#ifndef VRAUDIOSTATE_H
#define VRAUDIOSTATE_H


namespace MinVR {

class VRDataIndex;

/** 
*/
class VRAudioState {
public:
	VRAudioState(const VRDataIndex &internalIndex);
	virtual ~VRAudioState();

    
    // TODO:  Add special accessors with reasonable defaults.
    
    

	/** For advanced use, provides access to the VRDataIndex used internally by
	    MinVR, which may contain some additional data fields beyond those
	    accessible via this API. 
	 */
	const VRDataIndex& index() const;

private:

	const VRDataIndex &_index;
};


} // end namespace


#endif
