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

VRDisplay* GlfwWindowFactory::create(VRDataIndex& config, const std::string nameSpace) {
	//if (type == "glfw_display")
	//{
		GlfwWindow* window = new GlfwWindow(inputDevice);

		return window;
	//}

	return NULL;
}

} /* namespace MinVR */
