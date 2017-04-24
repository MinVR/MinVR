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

#include "../VRGraphicsState.h"
#include <main/VRGraphicsStateInternal.h>

namespace MinVR {

// This is the only constructor that needs to be implemented. The default and 
// copy constructors are listed as private simply to avoid accidental use.
VRGraphicsState::VRGraphicsState(VRGraphicsStateInternal *internalState) : _internal(internalState) {
}

const float * VRGraphicsState::getProjectionMatrix() const {
	return _internal->getProjectionMatrix();
}

const float * VRGraphicsState::getViewMatrix() const {
	return _internal->getViewMatrix();
}

const float * VRGraphicsState::getCameraPos() const {
	return _internal->getCameraPos();
}

bool VRGraphicsState::isInitialRenderCall() const {
	return _internal->isInitialRenderCall();
}

const int VRGraphicsState::getSharedContextId() const {
	return _internal->getSharedContextId();
}

const int VRGraphicsState::getWindowId() const {
	return _internal->getWindowId();
}

VRGraphicsStateInternal* VRGraphicsState::getInternal() const {
	return _internal;
}



} // end namespace

