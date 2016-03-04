/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/graphics/stereo/VRStereoFormatter.h>

namespace MinVR {

VRStereoFormatter::VRStereoFormatter() {
}

VRStereoFormatter::~VRStereoFormatter() {
}

void VRStereoFormatter::startRender() {
	for (int passNum = 0; passNum < getNumPasses(); passNum++)
	{
		preRenderPass(passNum);
		VRGraphicsWindowChild::startRender();
		postRenderPass(passNum);
	}
}

void VRStereoFormatter::addChild(VRGraphicsWindowChild* child) {
  VRDisplayNode::addChild(child);
}

} /* namespace MinVR */
