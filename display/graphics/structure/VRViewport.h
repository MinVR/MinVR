/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRVIEWPORT_H_
#define VRVIEWPORT_H_

#include "config/VRSerializable.h"

namespace MinVR {

class VRViewport : public VRSerializable {
public:
	VRViewport();
	VRViewport(double x, double y, double w, double h, bool usePercent = false);
	virtual ~VRViewport();

	VRViewport generateChild(const VRViewport& viewport);

	void serialize(VRDataIndex& index, std::string name) const;
	bool deserialize(VRDataIndex& index, std::string name);

	double getHeight() const {
		return m_height;
	}

	void setHeight(int height) {
		m_height = height;
	}

	double getWidth() const {
		return m_width;
	}

	void setWidth(int width) {
		m_width = width;
	}

	double getXOffset() const {
		return m_xOffset;
	}

	void setXOffset(int xOffset) {
		m_xOffset = xOffset;
	}

	double getYOffset() const {
		return m_yOffset;
	}

	void setYOffset(int yOffset) {
		m_yOffset = yOffset;
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
