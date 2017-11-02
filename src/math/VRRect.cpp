/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "VRRect.h"

namespace MinVR {

VRRect::VRRect() {
	m_xOffset = 0.0;
	m_yOffset = 0.0;
	m_width = 0.0;
	m_height = 0.0;
	m_usePercent = false;
}

VRRect::VRRect(float x, float y, float w, float h,
		bool usePercent) {
	m_xOffset = x;
	m_yOffset = y;
	m_width = w;
	m_height = h;
	m_usePercent = usePercent;
}

VRRect::~VRRect() {
	// TODO Auto-generated destructor stub
}

VRRect VRRect::generateChild(const VRRect& rect) {
	VRRect newRect(*this);
	newRect.m_usePercent = false;
	if (rect.m_usePercent)
	{
		newRect.m_xOffset += rect.m_xOffset*m_width;
		newRect.m_yOffset += rect.m_yOffset*m_height;
		newRect.m_width *= rect.m_width;
		newRect.m_height *= rect.m_height;
	}
	else
	{
		newRect.m_xOffset += rect.m_xOffset;
		newRect.m_yOffset += rect.m_yOffset;
		newRect.m_width = rect.m_width;
		newRect.m_height = rect.m_height;
	}

	return newRect;
}

void VRRect::write(VRDataIndex& index, std::string name) const {
	index.addData(name + "/xOffset", m_xOffset);
	index.addData(name + "/yOffset", m_yOffset);
	index.addData(name + "/width", m_width);
	index.addData(name + "/height", m_height);
	index.addData(name + "/usePercent", m_usePercent);
}

bool VRRect::read(VRDataIndex& index, std::string name, std::string nameSpace) {
	if (!index.exists(name, nameSpace)) {
		return false;
	}

	std::string newNamespace = index.getFullKey(name, nameSpace);

	m_xOffset = index.getValue("xOffset", newNamespace);
	m_yOffset = index.getValue("yOffset", newNamespace);
	m_width = index.getValue("width", newNamespace);
	m_height = index.getValue("height", newNamespace);
	m_usePercent = index.exists("usePercent", newNamespace) ? (int)index.getValue("usePercent", newNamespace) : 0;

	return true;
}

} /* namespace MinVR */
