/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "VRGraphicsWindowNode.h"

namespace MinVR {

VRGraphicsWindowNode::VRGraphicsWindowNode(const std::string &name, VRGraphicsToolkit *gfxToolkit, VRWindowToolkit *winToolkit, const VRWindowSettings &settings) : 
	VRDisplayNode(name), _gfxToolkit(gfxToolkit), _winToolkit(winToolkit), _settings(settings) 
{
	_windowID = _toolkit->createWindow(_settings);
}

VRGraphicsWindowNode::~VRGraphicsWindowNode() {
}

void VRGraphicsWindowNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler) {
	renderState->pushState();

	// Is this the kind of state information we expect to pass from one node to the next?
	renderState->addValue("WindowX", settings.xpos);
	renderState->addValue("WindowY", settings.ypos);
	renderState->addValue("WindowWidth", settings.width);
	renderState->addValue("WindowHeight", settings.height);

	_winToolkit->makeWindowCurrent(_windowID);
	_gfxToolkit->clearScreen();
	
	// windows should call the application programmer's context-level callback
	renderHandler->onVRRenderContext(renderState, this);

	if (m_children.size() == 0) {
		// if the window node is a leaf node, then call the onRenderScene callback		
		renderHandler->onVRRenderScene(renderState, this);
	}
	else {
		// otherwise, call render on all children, and they will call onRenderScene if they are leaves
		VRDisplayNode::render(renderState, renderHandler);
	}

	_gfxToolkit->flushGraphics();

	renderState->popState();
}

void VRGraphicsWindowNode::waitForRenderToComplete(VRDataIndex *renderState) {
	VRDisplayNode::waitForRenderToComplete(renderState);
	_winToolkit->makeWindowCurrent(_windowID);
	_gfxToolkit->finishGraphics();
}

void VRGraphicsWindowNode::displayTheFinishedRendering(VRDataIndex *renderState) {
	VRDisplayNode::displayTheFinishedRendering(renderState);
	_winToolkit->makeWindowCurrent(_windowID);
	_winToolkit->swapBuffers();
}



VRDisplayNode* VRGraphicsWindowFactory::create(VRMain *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) 
{
	std::string nodeNameSpace = nameSpace + "/" + valName;

	std::string type = config->getValue("Type", nodeNameSpace);
	if (type != getType()) {
		// This factory cannot create the type specified
		return NULL;
	}

	VRGraphicsToolkit *gfxToolkit = vrMain->getGraphicsToolkit(config->getValue("GraphicsToolkit", nodeNameSpace));
	VRWindowToolkit *winToolkit = vrMain->getWindowToolkit(config->getValue("WindowToolkit", nodeNameSpace));

	VRWindowSettings settings;
	settings.xpos = config->getValue("XPos", nodeNameSpace);
	settings.ypos = config->getValue("YPos", nodeNameSpace);
	settings.width = config->getValue("Width", nodeNameSpace);
	settings.height = config->getValue("Height", nodeNameSpace);
	settings.border = config->getValue("Border", nodeNameSpace);
	settings.caption = config->getValue("Caption", nodeNameSpace);
	settings.quadBuffered = config->getValue("QuadBuffered", nodeNameSpace);
	settings.gpuAffinity = config->getValue("GPUAffinity", nodeNameSpace);

	VRDisplayNode *node = new VRGraphicsWindowNode(valName, gfxToolkit, winToolkit, settings);

	std::vector<std::string> childrenNames = config->getValue("Children", nameSpace);
	for (std::vector<std::string>::iterator it = childrenNames.begin(); it < childrenNames.end(); ++it) {
		VRDisplayNode *child = vrMain->getFactory()->createDisplayNode(vrMain, config, *it, "/");
		if (child != NULL) {
			node->addChild(child);
		}
	}

	return node;
}


} /* namespace MinVR */
