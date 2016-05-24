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
  
	_gfxToolkit->setViewport(_rect);

	if (_children.size() == 0) {
		// if the viewport node is a leaf node, then call the onRenderScene callback		
		renderHandler->onVRRenderScene(renderState, this);
	}
	else {
		// otherwise, call render on all children, and they will call onRenderScene if they are leaves
		VRDisplayNode::render(renderState, renderHandler);
	}

  renderState->popState();
}




VRDisplayNode* VRViewportNodeFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace)
{
	std::string nodeNameSpace = config->validateNameSpace(nameSpace + valName);

	std::string type = config->getValue("Type", nodeNameSpace);
	if (type != "VRViewportNode") {
		// This factory cannot create the type specified
		return NULL;
	}

	VRGraphicsToolkit *gfxToolkit = vrMain->getGraphicsToolkit(config->getValue("GraphicsToolkit", nodeNameSpace));

	int xpos = config->getValue("XPos", nodeNameSpace);
	int ypos = config->getValue("YPos", nodeNameSpace);
	int width = config->getValue("Width", nodeNameSpace);
	int height = config->getValue("Height", nodeNameSpace);

	VRDisplayNode *node = new VRViewportNode(valName, gfxToolkit, VRRect(xpos, ypos, width, height));

	return node;
}


} /* namespace MinVR */
