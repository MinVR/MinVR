/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/graphics/stereo/VRSideBySideNode.h>

namespace MinVR {

VRSideBySideNode::VRSideBySideNode() {
	m_viewports[0] = VRViewport(0.0, 0.0, 0.5, 1.0, true);
	m_viewports[1] = VRViewport(0.5, 0.0, 0.5, 1.0, true);
}

VRSideBySideNode::~VRSideBySideNode() {
}

int VRSideBySideNode::getNumPasses() {
	return 2;
}

void VRSideBySideNode::preRenderPass(VRRenderer& renderer, int passNum) {
	m_viewportFormatter.preRender(renderer, m_viewports[passNum]);
}

void VRSideBySideNode::postRenderPass(VRRenderer& renderer, int passNum) {
	m_viewportFormatter.postRender(renderer);
}

} /* namespace MinVR */
