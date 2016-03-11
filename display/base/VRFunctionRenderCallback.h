/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRFUNCTIONRENDERCALLBACK_H_
#define VRFUNCTIONRENDERCALLBACK_H_

#include "display/base/VRRenderCallback.h"

namespace MinVR {

class VRFunctionRenderCallback : public VRRenderCallback {
public:
	typedef void (*MethodType)(VRRenderState&);

	VRFunctionRenderCallback(MethodType render, MethodType update = NULL);
	virtual ~VRFunctionRenderCallback();

	void updateFrame(VRRenderState& state);
	void render(VRRenderState& state);

private:
	MethodType m_renderMethod;
	MethodType m_updateMethod;
};

} /* namespace MinVR */

#endif /* VRFUNCTIONRENDERCALLBACK_H_ */
