/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/VRRenderState.h>

namespace MinVR {

VRRenderState::VRRenderState() : m_nameSpace("") {
}

VRRenderState::~VRRenderState() {
}

VRDataIndex& VRRenderState::getDataIndex() {
	return m_index;
}

} /* namespace MinVR */
