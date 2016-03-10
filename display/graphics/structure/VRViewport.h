/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRVIEWPORT_H_
#define VRVIEWPORT_H_

#include "display/graphics/VRGraphicsWindowChild.h"

namespace MinVR {

class VRViewport : public VRGraphicsWindowChild {
public:
	VRViewport();
	virtual ~VRViewport();

	void render(VRRenderer& renderer);

	virtual void addChild(VRGraphicsWindowChild* child);

	int getHeight() const {
		return m_height;
	}

	void setHeight(int height) {
		m_height = height;
	}

	int getWidth() const {
		return m_width;
	}

	void setWidth(int width) {
		m_width = width;
	}

	int getXOffset() const {
		return m_xOffset;
	}

	void setXOffset(int xOffset) {
		m_xOffset = xOffset;
	}

	int getYOffset() const {
		return m_yOffset;
	}

	void setYOffset(int yOffset) {
		m_yOffset = yOffset;
	}

private:
	int m_xOffset;
	int m_yOffset;
	int m_width;
	int m_height;
};

} /* namespace MinVR */

#endif /* VRVIEWPORT_H_ */
