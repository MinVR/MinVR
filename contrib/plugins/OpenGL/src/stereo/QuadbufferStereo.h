/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef QUADBUFFERSTEREO_H_
#define QUADBUFFERSTEREO_H_

#include "display/VRDisplayDevice.h"

namespace MinVR {

class QuadbufferStereo : public VRDisplayDevice {
public:
	QuadbufferStereo();
	virtual ~QuadbufferStereo();

	bool isQuadbuffered() { return true; }

	void finishRendering();

protected:
	void startRendering(const MinVR::VRRenderer& renderer, int x);
};

} /* namespace MinVR */

#endif /* QUADBUFFERSTEREO_H_ */
