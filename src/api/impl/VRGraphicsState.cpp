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

#include "api/VRGraphicsState.h"
#include <config/VRDataIndex.h>

namespace MinVR {


float VRGraphicsState::projMat[16] = {1.0, 0.0, 0.0, 0.0,  0.0, 1.0, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0,  0.0, 0.0, 0.0, 1.0};
float VRGraphicsState::viewMat[16] = {1.0, 0.0, 0.0, 0.0,  0.0, 1.0, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0,  0.0, 0.0, 0.0, 1.0};
float VRGraphicsState::eyePos[3] = {0.0, 0.0, 0.0};
int VRGraphicsState::defaultSharedContextID = 0;
int VRGraphicsState::defaultWindowID = 0;


VRGraphicsState::VRGraphicsState(const VRDataIndex &internalIndex) : _index(internalIndex) {

}

VRGraphicsState::~VRGraphicsState() {

}

const VRDataIndex& VRGraphicsState::index() const {
    return _index;
}


const float * VRGraphicsState::getProjectionMatrix() const {
    if (_index.exists("ProjectionMatrix")) {
        const VRFloatArray* mat = (const VRFloatArray*)_index.getValue("ProjectionMatrix");
        return &(*mat)[0];
    }
    return projMat;
}

const float * VRGraphicsState::getViewMatrix() const {
    if (_index.exists("ViewMatrix")) {
        const VRFloatArray* mat = (const VRFloatArray*)_index.getValue("ViewMatrix");
        return &(*mat)[0];
    }
    return viewMat;
}

const float * VRGraphicsState::getCameraPos() const {
    if (_index.exists("EyePosition")) {
        const VRFloatArray* vec = (const VRFloatArray*)_index.getValue("EyePosition");
        return &(*vec)[0];
    }
    return eyePos;
}

bool VRGraphicsState::isInitialRenderCall() const {
    bool initRender = (int)_index.getValue("InitRender","/") == 1;
    return initRender;
}

int VRGraphicsState::getSharedContextId() const {
    if (_index.exists("SharedContextId")) {
        return _index.getValue("SharedContextId");
    }
    else {
        return defaultSharedContextID;
    }
}

int VRGraphicsState::getWindowId() const {
    if (_index.exists("WindowID")) {
        return _index.getValue("WindowID");
    }
    else {
        return defaultWindowID;
    }
}



} // end namespace

