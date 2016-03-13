/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/graphics/VRGraphicsWindowNode.h>

namespace MinVR {

VRGraphicsWindowNode::~VRGraphicsWindowNode() {
}

void VRGraphicsWindowNode::render(VRRenderer& renderer) {
	startRender(renderer);
	waitForRenderComplete();
	synchronize();
}

void VRGraphicsWindowNode::startRender(VRRenderer& renderer) {
	renderer.pushState();
	setCurrentContext();
	renderer.getState().setValue("graphicsContextType", getContextType());
	renderer.updateFrame();
	VRDisplayNode::renderAtLeaf(renderer);
	flush();
	renderer.popState();
	clearCurrentContext();
}

void VRGraphicsWindowNode::waitForRenderComplete() {
	setCurrentContext();
	finish();
	clearCurrentContext();
}

void VRGraphicsWindowNode::synchronize() {
	setCurrentContext();
	swapBuffers();
	clearCurrentContext();
}

void VRGraphicsWindowNode::addChild(VRGraphicsWindowChild* child) {
	VRDisplayNode::addChild(child);
}

} /* namespace MinVR */
