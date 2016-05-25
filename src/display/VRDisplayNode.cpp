/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "VRDisplayNode.h"
#include <main/VRFactory.h>

namespace MinVR {

VRDisplayNode::VRDisplayNode(const std::string &name) : _name(name) {

}

VRDisplayNode::~VRDisplayNode() {
	clearChildren(true);
}

void VRDisplayNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler) {
    if (_children.size() > 0) {
		for (vector<VRDisplayNode*>::iterator it = _children.begin(); it != _children.end(); it++) {
			(*it)->render(renderState, renderHandler);
		}
	}
}

void VRDisplayNode::waitForRenderToComplete(VRDataIndex *renderState) {
	for (vector<VRDisplayNode*>::iterator it = _children.begin(); it != _children.end(); it++) {
		(*it)->waitForRenderToComplete(renderState);
	}
}

void VRDisplayNode::displayFinishedRendering(VRDataIndex *renderState) {
	for (vector<VRDisplayNode*>::iterator it = _children.begin(); it != _children.end(); it++) {
		(*it)->displayFinishedRendering(renderState);
	}
}

const std::vector<VRDisplayNode*>& VRDisplayNode::getChildren() const {
	return _children;
}

void VRDisplayNode::addChild(VRDisplayNode* child) {
	_children.push_back(child);
}

void VRDisplayNode::clearChildren(bool destroyChildren) {
	if (destroyChildren) {
		for (vector<VRDisplayNode*>::iterator it = _children.begin(); it != _children.end(); it++) {
				delete (*it);
		}
	}

	_children.clear();
}

void VRDisplayNode::createChildren(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) {
  std::string nodeNameSpace = config->validateNameSpace(nameSpace + valName);
  std::list<std::string> names = config->getValue(valName,nameSpace);
  for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
	std::string name = it->substr(nodeNameSpace.size());
	if (config->exists(*it, nodeNameSpace)){
	  VRDisplayNode *child = vrMain->getFactory()->createDisplayNode(vrMain, config, name, nodeNameSpace);
	  if (child != NULL) {
		addChild(child);
	  }
	}
  }	
}

} /* namespace MinVR */
