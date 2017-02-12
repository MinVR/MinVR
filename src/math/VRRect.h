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

private:
	float m_xOffset;
	float m_yOffset;
	float m_width;
	float m_height;
	bool m_usePercent;
};

} /* namespace MinVR */

#endif /* VRVIEWPORT_H_ */
