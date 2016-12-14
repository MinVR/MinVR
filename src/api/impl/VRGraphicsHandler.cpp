/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "api/VRGraphicsHandler.h"

namespace MinVR {

class VRGraphicsHandlerImpl : public VRRenderHandler {
public:
	VRGraphicsHandlerImpl(VRGraphicsHandler& handler) : handler(handler) {}

	void onVRRender(VRDataIndex* renderState, VRDisplayNode* callingNode) {
		// Wraps VRDataIndex inside VRGraphicsState
		if (renderState->exists("HasGraphicsContext", "/") && 1 == (int)renderState->getValue("HasGraphicsContext", "/")) {
			VRGraphicsState state(*renderState);
			std::string renderType = renderState->getValue("Render", "/");
			if (renderType == "Context") {
				handler.onVRRenderGraphicsContext(state);
			}
			else {
				handler.onVRRenderGraphics(state);
			}
		}
	}

private:
	VRGraphicsHandler& handler;
};

VRGraphicsHandler::VRGraphicsHandler() : VRRenderHandlerImpl(new VRGraphicsHandlerImpl(*this)) {
}

VRGraphicsHandler::~VRGraphicsHandler() {
}

} /* namespace MinVR */

