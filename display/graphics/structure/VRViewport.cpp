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
	m_xOffset = 0.0;
	m_yOffset = 0.0;
	m_width = 0.0;
	m_height = 0.0;
	m_usePercent = false;
}

VRViewport::VRViewport(double x, double y, double w, double h,
		bool usePercent) {
	m_xOffset = x;
	m_yOffset = y;
	m_width = w;
	m_height = h;
	m_usePercent = usePercent;
}

VRViewport::~VRViewport() {
	// TODO Auto-generated destructor stub
}

VRViewport VRViewport::generateChild(const VRViewport& viewport) {
	VRViewport newViewport(*this);
	newViewport.m_usePercent = false;
	if (viewport.m_usePercent)
	{
		newViewport.m_xOffset += viewport.m_xOffset*m_width;
		newViewport.m_yOffset += viewport.m_yOffset*m_height;
		newViewport.m_width *= viewport.m_width;
		newViewport.m_height *= viewport.m_height;
	}
	else
	{
		newViewport.m_xOffset += viewport.m_xOffset;
		newViewport.m_yOffset += viewport.m_yOffset;
		newViewport.m_width = viewport.m_width;
		newViewport.m_height = viewport.m_height;
	}
	return newViewport;
}

void VRViewport::serialize(VRDataIndex& index, std::string name) const {
	index.addData(name + "/xOffset", m_xOffset);
	index.addData(name + "/yOffset", m_yOffset);
	index.addData(name + "/width", m_width);
	index.addData(name + "/height", m_height);
	index.addData(name + "/usePercent", m_usePercent);
}

bool VRViewport::deserialize(VRDataIndex& index, std::string name) {
	if (!index.exists("xOffset", name)) {
		return false;
	}

	m_xOffset = index.getValue("xOffset", name);
	m_yOffset = index.getValue("yOffset", name);
	m_width = index.getValue("width", name);
	m_height = index.getValue("height", name);
	m_usePercent = (int)index.getValue("usePercent", name);

	return true;
}

} /* namespace MinVR */
