/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRVIEWPORTOGL_H_
#define VRVIEWPORTOGL_H_

#include "display/VRDisplayDevice.h"

namespace MinVR {

class VRViewportOgl: public VRDisplayDevice {
public:
	VRViewportOgl(int x, int y, int width, int height);
	virtual ~VRViewportOgl();

	int getXOffset();
	int getYOffset();
	int getWidth();
	int getHeight();

	void finishRendering();

protected:
	void startRendering(const MinVR::VRRenderer& renderer, int x);

private:
	int x, y, width, height;
};

} /* namespace MinVR */

#endif /* VRVIEWPORTOGL_H_ */
