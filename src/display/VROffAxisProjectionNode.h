/*
 * Copyright Regents of the University of Minnesota and Brown University, 2016.  This software is released under the following license: http://opensource.org/licenses/
 *
 * Code author(s):
 *		Dan Keefe, UMN
 *
 */

#ifndef VROFFAXISPROJECTIONNODE_H
#define VROFFAXISPROJECTIONNODE_H

#include <string>

#include <display/VRDisplayNode.h>
#include <main/VRFactory.h>
#include <math/VRMath.h>

namespace MinVR {


/** Generates projection and view matrices for off-axis projection used in tiled
    VR displays (e.g., Powerwall, wall of a Cave, 3DTV) that use head tracked
    perspective rendering.  Takes a CameraMatrix as input in the current
    RenderState and sets the ProjectionMatrix and ViewMatrix as output.
 */
class VROffAxisProjectionNode : public VRDisplayNode {
public:

	VROffAxisProjectionNode(const std::string &name, VRPoint3 topLeft, VRPoint3 botLeft, VRPoint3 topRight, VRPoint3 botRight,
		 float nearClip, float farClip);
	virtual ~VROffAxisProjectionNode();

	virtual std::string getType() const { return "VROffAxisProjectionNode"; }

	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

	static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

protected:

	VRPoint3 _topLeft;
	VRPoint3 _botLeft;
	VRPoint3 _topRight;
	VRPoint3 _botRight;
	float _nearClip;
	float _farClip;
};

} // end namespace

#endif

