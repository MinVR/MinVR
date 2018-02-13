/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRRECT_H_
#define VRRECT_H_

#include "config/VRWritable.h"
#include "VRMath.h"

namespace MinVR {

class VRRect : public VRWritable {
public:
	VRRect();
	VRRect(float x, float y, float w, float h, bool usePercent = false);
	virtual ~VRRect();

	VRRect generateChild(const VRRect& rect);

	void write(VRDataIndex& index, std::string name) const;
	bool read(VRDataIndex& index, std::string name, std::string nameSpace);

	float getHeight() const {
		return m_height;
	}

	void setHeight(float height) {
		m_height = height;
	}

	float getWidth() const {
		return m_width;
	}

	void setWidth(float width) {
		m_width = width;
	}

	float getX() const {
		return m_xOffset;
	}

	void setX(float x) {
		m_xOffset = x;
	}

	float getY() const {
		return m_yOffset;
	}

	void setY(float y) {
		m_yOffset = y;
	}

	bool isUsePercent() const {
		return m_usePercent;
	}

	void setUsePercent(bool usePercent) {
		m_usePercent = usePercent;
	}

	VRRect intersect(const VRRect& rect)
	{
		VRVector3 low(m_xOffset, m_yOffset, 0);
		VRVector3 rectLow(rect.getX(), rect.getY(), 0);
		VRVector3 high = low + VRVector3(m_width, m_height, 0);
		VRVector3 rectHigh = rectLow + VRVector3(rect.getWidth(), rect.getHeight(), 0);
		VRVector3 maxLow(low.x > rectLow.x ? low.x : rectLow.x, low.y > rectLow.y ? low.y : rectLow.y, 0);
		VRVector3 minHigh(high.x < rectHigh.x ? high.x : rectHigh.x, high.y < rectHigh.y ? high.y : rectHigh.y, 0);
		VRRect intersection(maxLow.x, maxLow.y, minHigh.x - maxLow.x > 0 ? minHigh.x - maxLow.x : 0.0f, minHigh.y - maxLow.y > 0 ? minHigh.y - maxLow.y : 0);
		return intersection;
	}

	float getArea()
	{
		return m_width*m_height;
	}

private:
	float m_xOffset;
	float m_yOffset;
	float m_width;
	float m_height;
	bool m_usePercent;
};

} /* namespace MinVR */

#endif /* VRVIEWPORT_H_ */
