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
#include "display/VRDisplayDevice.h"
#include "GlfwInputDevice.h"

namespace MinVR {

class GlfwWindowFactory : public SimpleVRDisplayFactory {
public:
	PLUGIN_API GlfwWindowFactory(GlfwInputDevice* inputDevice);
	PLUGIN_API virtual ~GlfwWindowFactory();

	PLUGIN_API VRDisplayDevice* createDisplay(const std::string type, const std::string name, VRDataIndex& config, VRDisplayDeviceFactory* factory);

private:
	GlfwInputDevice* inputDevice;
};

} /* namespace MinVR */

#endif /* GLFWWINDOWFACTORY_H_ */
