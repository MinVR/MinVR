/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/graphics/structure/VRViewportNode.h>

namespace MinVR {

VRViewportNode::VRViewportNode(const VRViewport& viewport) : m_viewportCalculator(true), m_viewport(viewport) {
}

VRViewportNode::~VRViewportNode() {
}

void VRViewportNode::render(VRRenderer& renderer) {
	renderer.pushState();
	m_viewportCalculator.calculate(renderer.getState(), m_viewport);
	renderAtLeaf(renderer);
	renderer.popState();
}

void VRViewportNode::addChild(VRGraphicsWindowChild* child) {
  VRDisplayNode::addChild(child);
}

} /* namespace MinVR */
