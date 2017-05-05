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
	_winToolkit->getFramebufferSize(_windowID, _framebufferWidth, _framebufferHeight);
  _valuesAdded.push_back("/WindowX");
  _valuesAdded.push_back("/WindowX");
  _valuesAdded.push_back("/WindowWidth");
  _valuesAdded.push_back("/WindowHeight");
  _valuesAdded.push_back("/FramebufferWidth");
  _valuesAdded.push_back("/FramebufferHeight");
  _valuesAdded.push_back("/SharedContextId");
  _valuesAdded.push_back("/WindowID");

}

VRGraphicsWindowNode::~VRGraphicsWindowNode() {
}

void VRGraphicsWindowNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler) {

  renderState->pushState();

	// Is this the kind of state information we expect to pass from one node to the next?
	renderState->addData("/WindowX", _settings.xpos);
	renderState->addData("/WindowY", _settings.ypos);
	renderState->addData("/WindowWidth", _settings.width);
	renderState->addData("/WindowHeight", _settings.height);
	renderState->addData("/FramebufferWidth", _framebufferWidth);
	renderState->addData("/FramebufferHeight", _framebufferHeight);
	renderState->addData("/SharedContextId", _settings.sharedContextGroupID);
	renderState->addData("/WindowID", _windowID);
  
	_winToolkit->makeWindowCurrent(_windowID);
	/*if (_settings.quadBuffered)
	{
		_gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACKRIGHT);
		//_gfxToolkit->clearScreen();
		_gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACKLEFT);
		//_gfxToolkit->clearScreen();
	}
	else
	{
		_gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACK);
		//_gfxToolkit->clearScreen();
	}*/
	
	// windows should call the application programmer's context-level callback
	renderHandler->onVRRenderContext(renderState, this);

	VRDisplayNode::render(renderState, renderHandler);

	_gfxToolkit->flushGraphics();

  renderState->popState();
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



VRDisplayNode* VRGraphicsWindowNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace)
{
	std::string nodeNameSpace = nameSpace;

    std::string gtk = config->getValue("GraphicsToolkit", nodeNameSpace);
	VRGraphicsToolkit *gfxToolkit = vrMain->getGraphicsToolkit(gtk);
    if (gfxToolkit == NULL) {
      std::cerr << "Cannot get the graphics toolkit named: " << gtk << std::endl;
    }

	std::string wtk = config->getValue("WindowToolkit", nameSpace);
    VRWindowToolkit *winToolkit = vrMain->getWindowToolkit(wtk);
    if (winToolkit == NULL) {
      std::cerr << "Cannot get the window toolkit named: " << wtk << std::endl;
    }

	VRWindowSettings settings;
	settings.xpos = config->getValue("XPos", nameSpace);
	settings.ypos = config->getValue("YPos", nameSpace);
	settings.width = config->getValue("Width", nameSpace);
	settings.height = config->getValue("Height", nameSpace);
	settings.border = (int)config->getValue("Border", nameSpace);
	settings.visible = (int)config->getValue("Visible", nameSpace);
	settings.caption = std::string(config->getValue("Caption", nameSpace));
	settings.quadBuffered = (int)config->getValue("QuadBuffered", nameSpace);
	settings.sharedContextGroupID = int(config->getValue("SharedContextGroupID", nameSpace));
	settings.contextVersionMajor = int(config->getValue("ContextVersionMajor", nameSpace));
	settings.contextVersionMinor = int(config->getValue("ContextVersionMinor", nameSpace));
	settings.rgbBits = int(config->getValue("RGBBits", nameSpace));
	settings.alphaBits = int(config->getValue("AlphaBits", nameSpace));
	settings.depthBits = int(config->getValue("DepthBits", nameSpace));
	settings.stencilBits = int(config->getValue("StencilBits", nameSpace));
	settings.fullScreen = int(config->getValue("FullScreen", nameSpace));
	settings.resizable = int(config->getValue("Resizable", nameSpace));
	settings.allowMaximize = int(config->getValue("AllowMaximize", nameSpace));
 	settings.gpuAffinity = int(config->getValue("UseGPUAffinity", nameSpace));
	settings.debugContext = int(config->getValue("UseDebugContext", nameSpace));
	settings.msaaSamples = int(config->getValue("MSAASamples", nameSpace));

  //	std::cout << "Window corners: " << settings.xpos << ", " << settings.ypos << ", " << settings.width << ", " << settings.height << std::endl;

	VRDisplayNode *node = new VRGraphicsWindowNode(nameSpace, gfxToolkit, winToolkit, settings);

	return node;
}


} /* namespace MinVR */
