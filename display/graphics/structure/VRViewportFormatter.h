/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRVIEWPORTFORMATTER_H_
#define VRVIEWPORTFORMATTER_H_

#include "display/VRRenderer.h"
#include "VRViewport.h"

namespace MinVR {

class VRViewportFormatter {
public:
	VRViewportFormatter();
	virtual ~VRViewportFormatter();

	void preRender(VRRenderer& renderer, const VRViewport& viewport);
	void postRender(VRRenderer& renderer);
};

} /* namespace MinVR */

#endif /* VRVIEWPORTFORMATTER_H_ */
