/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRFRAMECONTROLLER_H_
#define VRFRAMECONTROLLER_H_

#include "display/VRDisplayFrameAction.h"

namespace MinVR {

class VRFrameController {
public:
	virtual ~VRFrameController() {}

	virtual bool renderFrame(VRDisplayFrameAction& frameAction) = 0;
};

} /* namespace MinVR */

#endif /* VRFRAMECONTROLLER_H_ */
