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
	_windowID = _winToolkit->createWindow(_settings);
}

VRGraphicsWindowNode::~VRGraphicsWindowNode() {
}

void VRGraphicsWindowNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler) {
  //renderState->pushState();

	// Is this the kind of state information we expect to pass from one node to the next?
	renderState->addData("WindowX", _settings.xpos);
	renderState->addData("WindowY", _settings.ypos);
	renderState->addData("WindowWidth", _settings.width);
	renderState->addData("WindowHeight", _settings.height);

	_winToolkit->makeWindowCurrent(_windowID);
    _gfxToolkit->setViewport(VRRect(_settings.xpos, _settings.ypos, _settings.width, _settings.height));
	_gfxToolkit->clearScreen();
	
	// windows should call the application programmer's context-level callback
	renderHandler->onVRRenderContext(renderState, this);

	if (_children.size() == 0) {
		// if the window node is a leaf node, then call the onRenderScene callback		
		renderHandler->onVRRenderScene(renderState, this);
	}
	else {
		// otherwise, call render on all children, and they will call onRenderScene if they are leaves
		VRDisplayNode::render(renderState, renderHandler);
	}

	_gfxToolkit->flushGraphics();

  //renderState->popState();
}

void VRGraphicsWindowNode::waitForRenderToComplete(VRDataIndex *renderState) {
	VRDisplayNode::waitForRenderToComplete(renderState);
	_winToolkit->makeWindowCurrent(_windowID);
	_gfxToolkit->finishGraphics();
}

void VRGraphicsWindowNode::displayFinishedRendering(VRDataIndex *renderState) {
	VRDisplayNode::displayFinishedRendering(renderState);
	_winToolkit->makeWindowCurrent(_windowID);
	_winToolkit->swapBuffers(_windowID);
}



VRDisplayNode* VRGraphicsWindowNodeFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace)
{
	std::string nodeNameSpace = config->validateNameSpace(nameSpace + valName);

	std::string type = config->getValue("Type", nodeNameSpace);
	if (type != "VRGraphicsWindowNode") {
		// This factory cannot create the type specified
		return NULL;
	}

    std::string gtk = config->getValue("GraphicsToolkit", nodeNameSpace);
	VRGraphicsToolkit *gfxToolkit = vrMain->getGraphicsToolkit(gtk);
    if (gfxToolkit == NULL) {
      std::cerr << "Cannot get the graphics toolkit named: " << gtk << std::endl;
    }

    std::string wtk = config->getValue("WindowToolkit", nodeNameSpace);
    VRWindowToolkit *winToolkit = vrMain->getWindowToolkit(wtk);
    if (winToolkit == NULL) {
      std::cerr << "Cannot get the window toolkit named: " << wtk << std::endl;
    }

	VRWindowSettings settings;
	settings.xpos = config->getValue("XPos", nodeNameSpace);
	settings.ypos = config->getValue("YPos", nodeNameSpace);
	settings.width = config->getValue("Width", nodeNameSpace);
	settings.height = config->getValue("Height", nodeNameSpace);
	settings.border = (int)config->getValue("Border", nodeNameSpace);
    settings.caption = std::string(config->getValue("Caption", nodeNameSpace));
	settings.quadBuffered = (int)config->getValue("QuadBuffered", nodeNameSpace);
    settings.gpuAffinity = std::string(config->getValue("GPUAffinity", nodeNameSpace));

	VRDisplayNode *node = new VRGraphicsWindowNode(valName, gfxToolkit, winToolkit, settings);

    if (config->exists("Children", nodeNameSpace)) {
	    std::vector<std::string> childrenNames = config->getValue("Children", nodeNameSpace);
	    for (std::vector<std::string>::iterator it = childrenNames.begin(); it < childrenNames.end(); ++it) {
		    VRDisplayNode *child = vrMain->getFactory()->createDisplayNode(vrMain, config, *it, "/MinVR/");
		    if (child != NULL) {
			    node->addChild(child);
		    }
	    }
    }
	return node;
}


} /* namespace MinVR */
