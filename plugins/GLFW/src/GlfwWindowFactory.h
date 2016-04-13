/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GLFWWINDOWFACTORY_H_
#define GLFWWINDOWFACTORY_H_

#include "plugin/PluginFramework.h"
#include "display/nodes/graphics/VRGraphicsWindowNode.h"
#include "GlfwInputDevice.h"

namespace MinVR {

class GlfwWindowFactory : public VRGraphicsWindowFactory {
public:
	PLUGIN_API GlfwWindowFactory(GlfwInputDevice* inputDevice, VRSystem* vrSystem);
	PLUGIN_API virtual ~GlfwWindowFactory();

	PLUGIN_API VRGraphicsWindowNode* createWindow(VRDataIndex& config, const std::string nameSpace, std::string type);

	PLUGIN_API VRDisplayNode* create(VRFactory *mainFactory, VRDataIndex *config, const std::string &valName, const std::string &nameSpace);

private:
	GlfwInputDevice* inputDevice;
};

} /* namespace MinVR */

#endif /* GLFWWINDOWFACTORY_H_ */
