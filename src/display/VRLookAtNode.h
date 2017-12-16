/*
 * Copyright Regents of the University of Minnesota and Brown University, 2016.  This software is released under the following license: http://opensource.org/licenses/
 *
 * Code author(s):
 *		Dan Keefe, UMN
 *
 */

#ifndef VRLOOKATNODE_H
#define VRLOOKATNODE_H

#include <string>

#include <display/VRDisplayNode.h>
#include <main/VRFactory.h>
#include <math/VRMath.h>

namespace MinVR {


/** Serves as a stand-in for head tracking on systems that do not support head
    tracking.  This node can replace a VRHeadTrackingNode by adding a 
    HeadMatrix entry to the RenderState, where HeadMatrix is defined based upon
    the LookAt* values specified in a config file.
 */
class VRLookAtNode : public VRDisplayNode{
public:

	VRLookAtNode(const std::string &name, VRMatrix4 initialHeadMatrix);
	virtual ~VRLookAtNode();

	virtual std::string getType() const { return "VRLookAtNode"; }

	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

	static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);
protected:

	VRMatrix4 _headMatrix;
};

} // end namespace

#endif

