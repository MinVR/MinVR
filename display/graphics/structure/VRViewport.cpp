/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/graphics/structure/VRViewport.h>

namespace MinVR {

VRViewport::VRViewport() {

}

VRViewport::~VRViewport() {
}

void VRViewport::startRender() {
	//TODO: set viewport inside data index
	VRGraphicsWindowChild::startRender();
}

void VRViewport::addChild(VRGraphicsWindowChild* child) {
  VRDisplayNode::addChild(child);
}

} /* namespace MinVR */
