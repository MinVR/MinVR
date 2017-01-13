/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "VRGraphicsHandler.h"
#include "VRGraphicsStateInternal.h"

namespace MinVR {

void VRGraphicsHandler::onVRRenderScene(VRDataIndex* renderState,
		VRDisplayNode* callingNode) {
	// Wraps VRDataIndex inside VRGraphicsState
	VRGraphicsStateInternal state(renderState);
	onVRRenderGraphics(*(state.getAPIState()));
}

void VRGraphicsHandler::onVRRenderContext(VRDataIndex* renderState,
		VRDisplayNode* callingNode) {
	// Wraps VRDataIndex inside VRGraphicsState
	VRGraphicsStateInternal state(renderState);
	onVRRenderGraphicsContext(*(state.getAPIState()));
}

} /* namespace MinVR */
