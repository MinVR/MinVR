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

void VRGraphicsWindowNode::startRender() {
	setCurrentContext();
	//TODO: load data
	VRDisplayNode::startRender();
	flush();
}

void VRGraphicsWindowNode::waitForRenderComplete() {
	VRDisplayNode::waitForRenderComplete();
	finish();
}

void VRGraphicsWindowNode::synchronize() {
	VRDisplayNode::synchronize();
	swapBuffers();
}

void VRGraphicsWindowNode::addChild(VRGraphicsWindowChild* child) {
	VRDisplayNode::addChild(child);
}

} /* namespace MinVR */
