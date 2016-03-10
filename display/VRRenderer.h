/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRRENDERER_H_
#define VRRENDERER_H_

#include "config/VRDataIndex.h"

namespace MinVR {

class VRRenderer {
public:
	virtual ~VRRenderer() {}

	virtual void updateFrame() = 0;
	virtual void render() = 0;

	virtual VRDataIndex& getState() const = 0;
	virtual void pushState() = 0;
	virtual void popState() = 0;

};

} /* namespace MinVR */

#endif /* VRRENDERER_H_ */
