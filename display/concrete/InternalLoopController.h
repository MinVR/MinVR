/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef INTERNALLOOPCONTROLLER_H_
#define INTERNALLOOPCONTROLLER_H_

#include "display/VRFrameController.h"
#include <iostream>

namespace MinVR {

class InternalLoopController : public VRFrameController {
public:
	InternalLoopController() {}
	virtual ~InternalLoopController() {}

	bool renderFrame(VRDisplayFrameAction& frameAction)
	{
		while (frameAction.exec())
		{
			//std::cout << "frame" << std::endl;
		}

		return false;
	}
};

} /* namespace MinVR */

#endif /* INTERNALLOOPCONTROLLER_H_ */
