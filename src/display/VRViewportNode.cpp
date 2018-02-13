/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/VRViewportNode.h>
#include <display/VRGraphicsToolkit.h>

namespace MinVR {

VRViewportNode::VRViewportNode(const std::string &name, VRGraphicsToolkit *gfxToolkit, const VRRect& rect) :
	VRDisplayNode(name), _rect(rect), _gfxToolkit(gfxToolkit)  {
  _valuesAdded.push_back("ViewportX");
  _valuesAdded.push_back("ViewportY");
  _valuesAdded.push_back("ViewportWidth");
  _valuesAdded.push_back("ViewportHeight");
}

VRViewportNode::~VRViewportNode() {
}

void VRViewportNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler) {
  renderState->pushState();

	// Is this the kind of state information we expect to pass from one node to the next?
	renderState->addData("ViewportX", (int)_rect.getX());
	renderState->addData("ViewportY", (int)_rect.getY());
	renderState->addData("ViewportWidth", (int)_rect.getWidth());
	renderState->addData("ViewportHeight", (int)_rect.getHeight());
  
	_gfxToolkit->setSubWindow(_rect);

	VRDisplayNode::render(renderState, renderHandler);

    renderState->popState();
}




VRDisplayNode* VRViewportNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace)
{
	VRGraphicsToolkit *gfxToolkit = vrMain->getGraphicsToolkit(config->getValue("GraphicsToolkit", nameSpace));

	int xpos = config->getValue("XPos", nameSpace);
	int ypos = config->getValue("YPos", nameSpace);
	int width = config->getValue("Width", nameSpace);
	int height = config->getValue("Height", nameSpace);

	VRDisplayNode *node = new VRViewportNode(nameSpace, gfxToolkit, VRRect((float)xpos, (float)ypos, (float)width, (float)height));

	return node;
}


} /* namespace MinVR */
