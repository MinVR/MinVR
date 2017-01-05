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

void VRDisplayNode::render(VRDataIndex *renderState,
                           VRRenderHandler *renderHandler) {
  if (_children.size() > 0) {
		for (vector<VRDisplayNode*>::iterator it = _children.begin();
         it != _children.end(); it++) {
			(*it)->render(renderState, renderHandler);
		}
	} else {
		renderHandler->onVRRenderScene(renderState, this);
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

void VRDisplayNode::createChildren(VRMainInterface *vrMain,
                                   VRDataIndex *config,
                                   const std::string &nameSpace) {

  std::list<std::string> names = config->getValue(nameSpace);
  std::string validatedNameSpace = config->validateNameSpace(nameSpace);

  for (std::list<std::string>::const_iterator it = names.begin();
       it != names.end(); ++it) {

	  if (config->exists(*it, validatedNameSpace)){
		  VRDisplayNode *child =
        vrMain->getFactory()->create<VRDisplayNode>(vrMain,
                                                    config,
                                                    validatedNameSpace + *it);
      if (child != NULL) {
        addChild(child);
      }
    }
  }	
}

  
/// Returns a list of the values added to the render state by this
/// node, and its children nodes.
std::map<std::string,std::string> VRDisplayNode::getValuesAdded() {

  std::map<std::string,std::string> out;

  // Stick the node name with the values added.
  if (_valuesAdded.size() > 0) {
    for (std::list<std::string>::iterator it = _valuesAdded.begin();
         it != _valuesAdded.end(); it++) {
      out[*it] = _name + "(" + getType() + ")";
    }
  }

  // Look through all the children nodes, and append their values to
  // the list, with the current node name on the front.
  if (_children.size() > 0) {
		for (vector<VRDisplayNode*>::iterator it = _children.begin();
         it != _children.end(); it++) {

      std::map<std::string,std::string> childOut = (*it)->getValuesAdded();

      if (childOut.size() > 0) {
        for (std::map<std::string,std::string>::iterator jt = childOut.begin();
             jt != childOut.end(); jt++) {
          out[jt->first] = jt->second;
        }
      }
		}
	} 

  return out;
}


} /* namespace MinVR */
