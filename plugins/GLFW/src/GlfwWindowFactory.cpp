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
#include "display/nodes/graphics/structure/VRViewportNode.h"
#include "display/nodes/graphics/structure/VRTileNode.h"
#include "display/nodes/graphics/stereo/VRSideBySideNode.h"

namespace MinVR {

GlfwWindowFactory::GlfwWindowFactory(GlfwInputDevice* iDevice, VRSystem* vrSystem) : VRGraphicsWindowFactory(vrSystem), inputDevice(iDevice) {
}

GlfwWindowFactory::~GlfwWindowFactory() {
}

VRGraphicsWindowNode* GlfwWindowFactory::createWindow(VRDataIndex& config, const std::string nameSpace, std::string type) {

	if (type != "GLFW")
	{
		return NULL;
	}

	std::cout << nameSpace << std::endl;

	VRRect rect;
	rect.read(config, nameSpace, "");

	GlfwWindow* window = new GlfwWindow(rect);

	inputDevice->registerGlfwWindow(window);

	VRTile tile;
	tile.setTopLeft(VRPoint3(-0.5,  0.5, 0.0));
	tile.setTopRight(VRPoint3( 0.5,  0.5, 0.0));
	tile.setBottomLeft(VRPoint3(-0.5, -0.5, 0.0));
	tile.setBottomRight(VRPoint3( 0.5, -0.5, 0.0));
	tile.setNearClip(0.01);
	tile.setFarClip(100.0);

	//window->addChild(new VRSideBySideNode());

	return window;
}

} /* namespace MinVR */
