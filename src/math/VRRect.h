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
	VRRect(double x, double y, double w, double h, bool usePercent = false);
	virtual ~VRRect();

	VRRect generateChild(const VRRect& rect);

	void write(VRDataIndex& index, std::string name) const;
	bool read(VRDataIndex& index, std::string name, std::string nameSpace);

	double getHeight() const {
		return m_height;
	}

	void setHeight(double height) {
		m_height = height;
	}

	double getWidth() const {
		return m_width;
	}

	void setWidth(double width) {
		m_width = width;
	}

	double getX() const {
		return m_xOffset;
	}

	void setX(double x) {
		m_xOffset = x;
	}

	double getY() const {
		return m_yOffset;
	}

	void setY(double y) {
		m_yOffset = y;
	}

	bool isUsePercent() const {
		return m_usePercent;
	}

	void setUsePercent(bool usePercent) {
		m_usePercent = usePercent;
	}

private:
	double m_xOffset;
	double m_yOffset;
	double m_width;
	double m_height;
	bool m_usePercent;
};

} /* namespace MinVR */

#endif /* VRVIEWPORT_H_ */
