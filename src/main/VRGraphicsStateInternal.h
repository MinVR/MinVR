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

#ifndef VRGRAPHICSSTATEINTERNAL_H_
#define VRGRAPHICSSTATEINTERNAL_H_

#include <config/VRDataIndex.h>
#include <api/VRGraphicsState.h>

namespace MinVR {


/** This class is used internally by MinVR to hold raw graphics data and 
	objects during MinVR graphics rendering (i.e., during traversal of the
	display graph).  This class is also in charge of a creating and 
	maintaining a corresponding, simplified non-mutable VRGraphicsState 
	object that exposes needed state info (e.g., current projection matrix)
	to users via the MinVR API.
*/
class VRGraphicsStateInternal {
public:

	/** Constructs a VRGraphicsStateInternal and a corresponding VRGraphicsState 
		based on the data provided.
	*/
	VRGraphicsStateInternal(VRDataIndex *data);
	~VRGraphicsStateInternal();


    const float * getProjectionMatrix();
    const float * getViewMatrix();
    const float * getCameraPos();
	const int getSharedContextId() const;
	const int getWindowId() const;
    bool isInitialRenderCall();
  
  
	// Do we need something like this as well, or is access to the data index sufficient???
	// VRDisplayNode* getCurrentDisplayNode();


    /** Returns a pointer to the streamlined VREvent data type associated with this VREventAdvanced.
    */
	VRGraphicsState* getAPIState();


    /** Returns a pointer to the underlying raw data index. 
    */
	VRDataIndex* getDataIndex();

private:
	float* getMatrix(VRDataIndex* state, const std::string& name) const;

    VRGraphicsState state;
	VRDataIndex* dataIndex;
	float* projectionMatrix;
	float* viewMatrix;
	float* cameraPos;
};


} // end namespace


#endif
