/*
 * Copyright Regents of the University of Minnesota and Brown University, 2016.  This software is released under the following license: http://opensource.org/licenses/
 *
 * Code author(s):
 *		Dan Keefe, UMN
 *
 */

#ifndef VRTRACKEDLOOKATNODE_H
#define VRTRACKEDLOOKATNODE_H

#include <string>

#include <display/VRDisplayNode.h>
#include <main/VREventHandler.h>
#include <main/VRFactory.h>
#include <math/VRMath.h>

namespace MinVR {


/** 
 */
class VRTrackedLookAtNode : public VRDisplayNode, public VREventHandler {
public:

	VRTrackedLookAtNode(const std::string &name, const std::string &headTrackingEventName, VRMatrix4 initialHeadMatrix);
	virtual ~VRTrackedLookAtNode();

	virtual std::string getType() { return "VRTrackedLookAtNode"; }

	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

    virtual void onVREvent(const VREvent &event);
  
	static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

protected:

	VRMatrix4 _lookAtMatrix;
	std::string _trackingEvent;
};

} // end namespace

#endif

