/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRVIEWPORTNODE_H_
#define VRVIEWPORTNODE_H_

#include "display/VRDisplayNode.h"
#include "math/VRRect.h"
#include "display/factories/VRDisplayFactory.h"
#include "main/VRSystem.h"

namespace MinVR {

class VRViewportNode : public VRDisplayNode {
public:
	VRViewportNode(const std::string &name, VRGraphicsToolkit, *gfxToolkit, const VRRect& rect);
	virtual ~VRViewportNode();

	virtual std::string getType() { return "VRViewportNode"; }

	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

private:
	VRRect _rect;
	VRGraphicsToolkit *_gfxToolkit;
};


class VRViewportNodeFactory : public VRDisplayNodeFactory {
public:
	virtual ~VRViewportNodeFactory() {}

	VRDisplayNode* create(VRMain *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace);
};


} /* namespace MinVR */

#endif /* VRVIEWPORTNODE_H_ */
