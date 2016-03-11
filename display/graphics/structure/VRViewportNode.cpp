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
	VRViewport* currentViewport = &m_viewport;

	VRViewport modifiedViewport;
	if (renderer.getState().getValue("viewport", modifiedViewport))
	{
		modifiedViewport = modifiedViewport.generateChild(m_viewport);
		currentViewport = &modifiedViewport;
	}

	renderer.pushState();
	VRDataIndex& state = renderer.getState().getDataIndex();
	renderer.getState().setValue("viewport", *currentViewport);
	if (getChildren().size() > 0)
	{
		VRGraphicsWindowChild::render(renderer);
	}
	else
	{
		renderer.render();
	}

	renderer.popState();
}

void VRViewportNode::addChild(VRGraphicsWindowChild* child) {
  VRDisplayNode::addChild(child);
}

} /* namespace MinVR */
