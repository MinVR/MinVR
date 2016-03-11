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
	// TODO Auto-generated destructor stub
}

void VRViewport::serialize(VRDataIndex& index, std::string name) const {
	index.addData(name + "/xOffset", m_xOffset);
	index.addData(name + "/yOffset", m_yOffset);
	index.addData(name + "/width", m_width);
	index.addData(name + "/height", m_height);
}

void VRViewport::deserialize(VRDataIndex& index, std::string name) {
	m_xOffset = index.getValue("xOffset", name);
	m_yOffset = index.getValue("yOffset", name);
	m_width = index.getValue("width", name);
	m_height = index.getValue("height", name);
}

} /* namespace MinVR */
