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
#include <main/VREventHandler.h>
#include <main/VRFactory.h>
#include <math/VRMath.h>

namespace MinVR {


/** 
 */
class VROffAxisProjectionNode : public VRDisplayNode, public VREventHandler {
public:

	VROffAxisProjectionNode(const std::string &name, VRVector3 topLeft, VRVector3 botLeft, VRVector3 topRight, VRVector3 botRight, 
		float interOcularDist, const std::string &headTrackingEventName, VRMatrix4 initialHeadMatrix, double nearClip, double farClip);
	virtual ~VROffAxisProjectionNode();

	virtual std::string getType() { return "VROffAxisProjectionNode"; }

	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

	virtual void onVREvent(const std::string &eventName, VRDataIndex *eventData);

protected:

	VRVector3 _topLeft;
	VRVector3 _botLeft;
	VRVector3 _topRight;
	VRVector3 _botRight;
	float     _iod;
	VRMatrix4 _headMatrix;
	std::string _trackingEvent;
	double _nearClip;
	double _farClip;
};


/** Small factory for creating this specific type of display node.  To be registered as a 
    "sub-factory" with the main VRFactory. 
 */
class VROffAxisProjectionNodeFactory : public VRDisplayNodeFactory {
public:
	virtual ~VROffAxisProjectionNodeFactory() {}

	VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace);
};

} // end namespace

#endif

