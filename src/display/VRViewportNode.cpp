/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/nodes/graphics/structure/VRViewportNode.h>
#include "VRTile.h"
#include "display/nodes/graphics/structure/VRTileNode.h"
#include "display/nodes/scope/VRStateScopeNode.h"

namespace MinVR {

VRViewportNode::VRViewportNode(const std::string &name, VRGraphicsToolkit, *gfxToolkit, const VRRect& rect) : 
	VRDisplayNode(name), _gfxToolkit(gfxToolkit), _rect(rect) {
}

VRViewportNode::~VRViewportNode() {
}

void VRViewportNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler) {
	renderState->pushState();

	// Is this the kind of state information we expect to pass from one node to the next?
	renderState->addValue("ViewportX", _rect.getX());
	renderState->addValue("ViewportY", _rect.getY());
	renderState->addValue("ViewportWidth", _rect.getWidth());
	renderState->addValue("ViewportHeight", _rect.getHeight());

	_gfxToolkit->setViewport(_rect.getX(), _rect.getY(), _rect.getWidth(), _rect.getHeight());

	if (m_children.size() == 0) {
		// if the viewport node is a leaf node, then call the onRenderScene callback		
		renderHandler->onVRRenderScene(renderState, this);
	}
	else {
		// otherwise, call render on all children, and they will call onRenderScene if they are leaves
		VRDisplayNode::render(renderState, renderHandler);
	}

	renderState->popState();
}




VRDisplayNode* VRViewportNodeFactory::create(VRMain *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) 
{
	std::string nodeNameSpace = nameSpace + "/" + valName;

	std::string type = config->getValue("Type", nodeNameSpace);
	if (type != getType()) {
		// This factory cannot create the type specified
		return NULL;
	}

	VRGraphicsToolkit *gfxToolkit = vrMain->getGraphicsToolkit(config->getValue("GraphicsToolkit", nodeNameSpace));

	int xpos = config->getValue("XPos", nodeNameSpace);
	int ypos = config->getValue("YPos", nodeNameSpace);
	int width = config->getValue("Width", nodeNameSpace);
	int height = config->getValue("Height", nodeNameSpace);

	VRDisplayNode *node = new VRViewportNode(valName, gfxToolkit, VRRect(xpos, ypos, width, height));

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
