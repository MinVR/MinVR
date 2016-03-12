/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/graphics/structure/VRTile.h>

namespace MinVR {

VRTile::VRTile() : m_topLeft(), m_topRight(), m_bottomLeft(), m_bottomRight(), m_nearClip(0.0), m_farClip(0.0) {
}

VRTile::~VRTile() {
}

void VRTile::serialize(VRDataIndex& index, std::string name) const {
	VRPoint3 topLeft(m_topLeft);
	index.addData(name + "/topLeft", topLeft);
	VRPoint3 topRight(m_topRight);
	index.addData(name + "/topRight", topRight);
	VRPoint3 bottomLeft(m_bottomLeft);
	index.addData(name + "/bottomLeft", bottomLeft);
	VRPoint3 bottomRight(m_bottomRight);
	index.addData(name + "/bottomRight", bottomRight);
	index.addData(name + "/nearClip", m_nearClip);
	index.addData(name + "/farClip", m_farClip);
}

bool VRTile::deserialize(VRDataIndex& index, std::string name) {
	if (!index.exists("topLeft", name)) {
		return false;
	}

	m_topLeft = index.getValue("topLeft", name);
	m_topRight = index.getValue("topRight", name);
	m_bottomLeft = index.getValue("bottomLeft", name);
	m_bottomRight = index.getValue("bottomRight", name);
	m_nearClip = index.getValue("nearClip", name);
	m_farClip = index.getValue("farClip", name);

	return true;
}

VRTile VRTile::modifyWithViewport(const VRViewport& oldViewport,
		const VRViewport& newViewport) {
	VRVector3 u = (m_topRight - m_topLeft).normalize();
	VRVector3 v = (m_topLeft - m_bottomLeft).normalize();

	double widthRatio = newViewport.getWidth()/oldViewport.getWidth();
	double heightRatio = newViewport.getHeight()/oldViewport.getHeight();

	double xOffsetRatio = (newViewport.getXOffset()-oldViewport.getXOffset())/oldViewport.getWidth();
	double yOffsetRatio = (newViewport.getYOffset()-oldViewport.getYOffset())/oldViewport.getHeight();

	m_bottomLeft = m_bottomLeft + u*xOffsetRatio + v*yOffsetRatio;

	m_bottomRight = m_bottomLeft + u*widthRatio;
	m_topLeft = m_bottomLeft + v*heightRatio;
	m_topRight = m_bottomRight + v*heightRatio;

	return *this;
}

} /* namespace MinVR */


