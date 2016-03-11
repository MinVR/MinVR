/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/base/VRBasicRenderer.h>
#include <sstream>

namespace MinVR {

VRBasicRenderer::VRBasicRenderer(VRRenderCallback& renderCallback) : m_nodeNum(0) {
	m_nameSpaces.push_back("/");
	m_state.setNameSpace("/");
}

VRBasicRenderer::~VRBasicRenderer() {
}

void VRBasicRenderer::updateFrame() {
	m_renderCallback->updateFrame(m_state);
}

void VRBasicRenderer::render() {
	m_renderCallback->render(m_state);
}

VRRenderState& VRBasicRenderer::getState() {
	return m_state;
}

void VRBasicRenderer::pushState() {
	m_nodeNum++;
	std::stringstream ss;
	ss << "node" << m_nodeNum;
	m_nameSpaces.push_back(ss.str());
	m_state.setNameSpace(m_nameSpaces[m_nameSpaces.size() - 1]);
}

void VRBasicRenderer::popState() {
	m_nameSpaces.pop_back();
	m_state.setNameSpace(m_nameSpaces[m_nameSpaces.size() - 1]);
}

void VRBasicRenderer::resetState() {
	m_state = VRRenderState();
	m_nameSpaces.push_back("/");
	m_state.setNameSpace("/");
}

} /* namespace MinVR */
