/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/graphics/structure/VRViewportNode.h>

namespace MinVR {

VRViewportNode::VRViewportNode() {
}

VRViewportNode::~VRViewportNode() {
}

void VRViewportNode::render(VRRenderer& renderer) {
	m_viewportFormatter.preRender(renderer, m_viewport);

	renderAtLeaf(renderer);

	m_viewportFormatter.postRender(renderer);
}

void VRViewportNode::addChild(VRGraphicsWindowChild* child) {
  VRDisplayNode::addChild(child);
}

} /* namespace MinVR */
