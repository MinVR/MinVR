/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <GlfwWindowFactory.h>
#include "GlfwWindow.h"
#include <iostream>
#include "config/VRDataIndex.h"

namespace MinVR {

GlfwWindowFactory::GlfwWindowFactory(GlfwInputDevice* iDevice) : inputDevice(iDevice) {
}

GlfwWindowFactory::~GlfwWindowFactory() {
}

VRDisplayDevice* GlfwWindowFactory::createDisplay(const std::string type, const std::string name,
		VRDataIndex& config, VRDisplayDeviceFactory* factory) {
	if (type == "glfw_display")
	{
		int xOffset = config.getValue("xOffset", name);
		int yOffset = config.getValue("yOffset", name);
		int width = config.getValue("width", name);
		int height = config.getValue("height", name);

		GlfwWindow* window = new GlfwWindow(xOffset, yOffset, width, height);
		if (config.exists("allowThreading", name))
		{
			int allowThreading = config.getValue("allowThreading", name);
			window->setAllowThreading(allowThreading == 1);
		}
		window->setInputDevice(inputDevice);

		return window;
	}

	return NULL;
}

} /* namespace MinVR */
