/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRSTEREONODE_H_
#define VRSTEREONODE_H_

#include "display/graphics/VRGraphicsWindowChild.h"

namespace MinVR {

class VRStereoNode : public VRGraphicsWindowChild {
public:
	VRStereoNode();
	virtual ~VRStereoNode();

	virtual void render(VRRenderer& renderer);
	virtual void addChild(VRGraphicsWindowChild* child);

protected:
	virtual int getNumPasses() = 0;
	virtual void preRenderPass(VRRenderer& renderer, int passNum) = 0;
	virtual void postRenderPass(VRRenderer& renderer, int passNum) = 0;
};

} /* namespace MinVR */

#endif /* VRSTEREOFORMATTER_H_ */
