/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef SIDEBYSIDESTEREO_H_
#define SIDEBYSIDESTEREO_H_

#include "display/VRDisplayDevice.h"

namespace MinVR {

class SideBySideStereo : public VRDisplayDevice {
public:
	SideBySideStereo();
	virtual ~SideBySideStereo();

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

#endif /* SIDEBYSIDESTEREO_H_ */
