/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRRENDERCALLBACK_H_
#define VRRENDERCALLBACK_H_

#include "display/VRRenderState.h"

namespace MinVR {

class VRRenderCallback {
public:
	virtual ~VRRenderCallback() {}

	virtual void updateFrame(VRRenderState& state) = 0;
	virtual void render(VRRenderState& state) = 0;
};

} /* namespace MinVR */

#endif /* VRRENDERCALLBACK_H_ */
