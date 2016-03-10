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
	m_xOffset = 0;
	m_yOffset = 0;
	m_width = 0;
	m_height = 0;
}

VRViewport::~VRViewport() {
}

void VRViewport::render(VRRenderer& renderer) {
	renderer.pushState();
	VRDataIndex& state = renderer.getState().getDataIndex();
	renderer.getState().setValue<int>("viewportX", m_xOffset);
	renderer.getState().setValue<int>("viewportY", m_yOffset);
	renderer.getState().setValue<int>("viewportWidth", m_width);
	renderer.getState().setValue<int>("viewportHeight", m_height);
	VRGraphicsWindowChild::render(renderer);
	renderer.popState();
}

void VRViewport::addChild(VRGraphicsWindowChild* child) {
  VRDisplayNode::addChild(child);
}

} /* namespace MinVR */
