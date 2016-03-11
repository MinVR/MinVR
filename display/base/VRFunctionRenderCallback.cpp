/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/base/VRFunctionRenderCallback.h>

namespace MinVR {

VRFunctionRenderCallback::VRFunctionRenderCallback(MethodType render, MethodType update) {
	m_renderMethod = render;
	m_updateMethod = update;
}

VRFunctionRenderCallback::~VRFunctionRenderCallback() {
}

void VRFunctionRenderCallback::updateFrame(VRRenderState& state) {
	if (m_updateMethod != NULL)
	{
		(*m_updateMethod)(state);
	}
}

void VRFunctionRenderCallback::render(VRRenderState& state) {
	(*m_renderMethod)(state);
}

} /* namespace MinVR */
