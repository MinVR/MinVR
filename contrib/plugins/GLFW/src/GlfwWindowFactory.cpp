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
#include "display/graphics/structure/VRViewportNode.h"
#include "display/graphics/structure/VRTileNode.h"
#include "display/graphics/stereo/VRSideBySideNode.h"

namespace MinVR {

GlfwWindowFactory::GlfwWindowFactory(GlfwInputDevice* iDevice) : inputDevice(iDevice) {
}

GlfwWindowFactory::~GlfwWindowFactory() {
}

VRDisplay* GlfwWindowFactory::create(VRDataIndex& config, const std::string nameSpace) {

	//if (type == "glfw_display")
	//{
		GlfwWindow* window = new GlfwWindow(inputDevice);

		/*Window1_Viewport1_TopLeft           (-0.65, 0.5, 0.0)
		Window1_Viewport1_TopRight          ( 0.65, 0.5, 0.0)
		Window1_Viewport1_BotLeft           (-0.65,-0.5, 0.0)
		Window1_Viewport1_BotRight          ( 0.65,-0.5, 0.0)
		Window1_Viewport1_NearClip          0.01
		Window1_Viewport1_FarClip           100.0*/

		VRTile tile;
		tile.setTopLeft(VRPoint3(-0.5,  0.5, 0.0));
		tile.setTopRight(VRPoint3( 0.5,  0.5, 0.0));
		tile.setBottomLeft(VRPoint3(-0.5, -0.5, 0.0));
		tile.setBottomRight(VRPoint3( 0.5, -0.5, 0.0));
		tile.setNearClip(0.01);
		tile.setFarClip(100.0);

		VRViewportNode* viewportNode = new VRViewportNode(VRViewport(0,0,800,400));
		VRTileNode* tileNode = new VRTileNode(tile);
		viewportNode->addChild(tileNode);
		tileNode->addChild(new VRSideBySideNode());
		window->addChild(viewportNode);

		return window;
	//}

	return NULL;
}

} /* namespace MinVR */
