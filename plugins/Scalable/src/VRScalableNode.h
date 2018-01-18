/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Ben Knorlein
 *
 * The software in this file uses a proprietary library from Scalable Display Technologies of Cambridge, Massachusetts (scalabledisplay.com).
 */

#ifndef VRSCALABLENODE_H
#define VRSCALABLENODE_H

#include <display/VRDisplayNode.h>
#include <main/VRFactory.h>
#include <plugin/VRPlugin.h>

//  The EasyBlendSDK is a proprietary library from Scalable Display Technologies of Cambridge, Massachusetts (scalabledisplay.com).
#include "EasyBlendSDK.h"

namespace MinVR {

/** OpenGL Implementation for the MinVR VRGraphicsToolkit abstraction.
 */
class VRScalableNode : public VRDisplayNode {
public:
	PLUGIN_API VRScalableNode(const std::string &name, const std::string meshName, double _near, double _far);

	PLUGIN_API virtual ~VRScalableNode();

	PLUGIN_API virtual std::string getType() const { return "VRScalableNode"; }

	PLUGIN_API virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

	PLUGIN_API virtual void displayFinishedRendering(VRDataIndex *renderState);

	//virtual void onVREvent(const std::string &eventName, VRDataIndex *eventData);
	PLUGIN_API static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

private:
	EasyBlendSDK_Mesh *gMSDK;
	std::string meshName;
	bool isInitialized;
	bool isLeftEye;
	double near;
	double far;
};

} // end namespace

#endif
