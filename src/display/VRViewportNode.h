/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRVIEWPORTNODE_H_
#define VRVIEWPORTNODE_H_

#include <display/VRDisplayNode.h>
#include <display/VRGraphicsToolkit.h>
#include <main/VRFactory.h>
#include <math/VRRect.h>

namespace MinVR {

class VRViewportNode : public VRDisplayNode {
public:
	VRViewportNode(const std::string &name, VRGraphicsToolkit *gfxToolkit, const VRRect& rect);
	virtual ~VRViewportNode();

	virtual std::string getType() const { return "VRViewportNode"; }

	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

	static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

private:
	VRRect _rect;
	VRGraphicsToolkit *_gfxToolkit;
};



} /* namespace MinVR */

#endif /* VRVIEWPORTNODE_H_ */
