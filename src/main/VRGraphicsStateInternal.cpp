/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "VRGraphicsStateInternal.h"
#include "math/VRMath.h"

namespace MinVR {

VRGraphicsStateInternal::VRGraphicsStateInternal(VRDataIndex *data)
	: state(this), dataIndex(data), projectionMatrix(NULL), viewMatrix(NULL), cameraPos(NULL) {
}

VRGraphicsStateInternal::~VRGraphicsStateInternal() {
	if (projectionMatrix) { delete[] projectionMatrix; }
	if (viewMatrix) { delete[] viewMatrix; }
	if (cameraPos) { delete[] cameraPos; }
}

float* VRGraphicsStateInternal::getMatrix(VRDataIndex* state, const std::string& name) const {
	float* val = new float[16];

    /**
    std::cout << "DataIndex = " << std::endl;
    std::cout << state->printStructure() << std::endl;
    **/
     
	VRMatrix4 mat;
	if (dataIndex->exists(name, "/")) {
		mat = dataIndex->getValue(name, "/");
	}
    //else {
    //    std::cout << name << ": NOT FOUND" << std::endl;
    //}
    
    //std::cout << name;
	for (int f = 0; f < 16; f++) {
		val[f] = mat.getArray()[f];
        //std::cout << " [" << f << "]=" << val[f];
	}
    //std::cout << std::endl;

	return val;
}

const float * VRGraphicsStateInternal::getProjectionMatrix() {
	if (!projectionMatrix) {
		projectionMatrix = getMatrix(dataIndex, "ProjectionMatrix");
	}

	return projectionMatrix;
}

const float * VRGraphicsStateInternal::getViewMatrix() {
	if (!viewMatrix) {
		viewMatrix = getMatrix(dataIndex, "ViewMatrix");
	}

	return viewMatrix;
}

const float* VRGraphicsStateInternal::getCameraPos() {
	if (!cameraPos) {
		VRPoint3 pe;
		if (dataIndex->exists("EyePosition","/")) {
			pe = dataIndex->getValue("EyePosition","/");
		}

		cameraPos = new float[3];
		cameraPos[0] = pe.x;
		cameraPos[1] = pe.y;
		cameraPos[2] = pe.z;
	}

	return cameraPos;
}

VRGraphicsState* VRGraphicsStateInternal::getAPIState() {
  return &state;
}

VRDataIndex* VRGraphicsStateInternal::getDataIndex() {
    return dataIndex;
  }
 
const int VRGraphicsStateInternal::getWindowId() const {
	return (int)dataIndex->getValue("WindowID", "/");
}

const int VRGraphicsStateInternal::getSharedContextId() const {
	return (int)dataIndex->getValue("SharedContextId", "/");
}

bool VRGraphicsStateInternal::isInitialRenderCall() {
	bool initRender = (int)dataIndex->getValue("InitRender","/") == 1;
	return initRender;
}

}
