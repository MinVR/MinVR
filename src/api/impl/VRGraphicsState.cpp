/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "api/VRGraphicsState.h"
#include "math/VRMath.h"

namespace MinVR {

class VRGraphicsState::VRGraphicsStateInternal {
public:
	VRGraphicsStateInternal(VRDataIndex& state) : state(&state), projectionMatrix(NULL), viewMatrix(NULL), cameraPos(NULL) {}

	~VRGraphicsStateInternal() {
		if (projectionMatrix) { delete[] projectionMatrix; }
		if (viewMatrix) { delete[] viewMatrix; }
		if (cameraPos) { delete[] cameraPos; }
	}

	void getMatrix(const std::string& name, float*& matrix) const {
		if (matrix == NULL) {
			matrix = new float[16];
		}

		VRMatrix4 mat;
		if (state->exists(name, "/")) {
			mat = state->getValue(name, "/");
		}

		for (int f = 0; f < 16; f++) {
			matrix[f] = mat.m[f];
		}
	}

	void getVector(const std::string& name, float*& vector) const {
		if (vector == NULL) {
			vector = new float[3];
		}

		VRVector3 vec;
		if (state->exists(name,"/")) {
			vec = state->getValue(name,"/");
		}

		vector[0] = vec.x;
		vector[1] = vec.y;
		vector[2] = vec.z;
	}

	VRDataIndex* state;
	float* projectionMatrix;
	float* viewMatrix;
	float* cameraPos;
};

VRGraphicsState::VRGraphicsState(VRDataIndex& state)
	: _(*(new VRGraphicsStateInternal(state))) {
}

VRGraphicsState::~VRGraphicsState() {
	delete &_;
}

const float * VRGraphicsState::getProjectionMatrix() const {
	_.getMatrix("ProjectionMatrix", _.projectionMatrix);
	return _.projectionMatrix;
}

const float * VRGraphicsState::getViewMatrix() const {
	_.getMatrix("ViewMatrix", _.viewMatrix);
	return _.viewMatrix;
}

const float* VRGraphicsState::getCameraPos() const {
	_.getVector("EyePosition", _.cameraPos);
	return _.cameraPos;
}

bool VRGraphicsState::isInitialRenderCall() const {
	bool initRender = (int)_.state->getValue("InitRender","/") == 1;
	return initRender;
}

}
