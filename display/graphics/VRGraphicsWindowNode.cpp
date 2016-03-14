/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/graphics/VRGraphicsWindowNode.h>

namespace MinVR {

VRGraphicsWindowNode::VRGraphicsWindowNode(const VRViewport& viewport) : m_viewport(viewport), m_viewportCalculator(true), m_hasTile(false) {

}

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
	updateState(renderer.getState());
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

void VRGraphicsWindowNode::updateState(VRRenderState& state) {
	VRViewport vp = m_viewportCalculator.calculate(state, m_viewport);
	vp.setXOffset(0);
	vp.setYOffset(0);
	state.writeValue("viewport", vp);

	if (m_hasTile)
	{
		state.writeValue("tile", m_tile);
	}
}

} /* namespace MinVR */
