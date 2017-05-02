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

  std::string validatedNameSpace = config->validateNameSpace(nameSpace);

  std::list<std::string> names =
    config->selectByAttribute("displaynodeType", "*", validatedNameSpace, true);

  for (std::list<std::string>::const_iterator it = names.begin();
       it != names.end(); ++it) {

    // We only want to do this for direct children. The grandchildren
    // and their progeny will be addressed in turn.
    if (VRDataIndex::isChild(nameSpace, *it) == 1) {

      VRDisplayNode *child =
        vrMain->getFactory()->create<VRDisplayNode>(vrMain, config, *it);
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

void VRDisplayNode::auditValues(std::list<std::string> valuesSupplied) {
  // First check to see if all of the values needed appear in the
  // input list.
  bool found;

  if ((_valuesNeeded.size() > 0) && (valuesSupplied.size() > 0)) {
    for (std::list<std::string>::iterator it = _valuesNeeded.begin();
         it != _valuesNeeded.end(); it++) {

      found = false;
      for (std::list<std::string>::iterator jt = valuesSupplied.begin();
           jt != valuesSupplied.end(); jt++) {
        found = found || ((*it).compare(*jt) == 0);
      }
      // If we haven't found this needed value, throw an error.
      if (!found)
        throw std::runtime_error("Needed " + (*it) + " but didn't get it, in " +
                                 getName() + ":" + getType());
    }
  }
  // Then add the valuesAdded to the input list and pass along to the
  // children nodes.
  valuesSupplied.insert(valuesSupplied.end(),
                       _valuesAdded.begin(), _valuesAdded.end());

  if (_children.size() > 0) {
    for (std::vector<VRDisplayNode*>::iterator it =  _children.begin();
         it != _children.end(); it++) {
      (*it)->auditValues(valuesSupplied);
    }
  }
}

std::string VRDisplayNode::printNode(const std::string &prefix) const {

  std::string name;
  if (_name.size() > 48) {
    name = _name.substr(0,15) + "..." +
      _name.substr(_name.size() - 33, std::string::npos);

  } else {
    name = _name;
  }

  std::string out = prefix + "<displayNode:" + name + ">";

  out += "\n" + prefix + "   Values Added";
  for (std::list<std::string>::const_iterator it = _valuesAdded.begin();
       it != _valuesAdded.end(); it++) {
    out += "\n" + prefix + "     " + *it;
  }
  if (_valuesAdded.empty()) out += "\n" + prefix + "     <none>";

  out += "\n" + prefix + "   Values Needed";
  for (std::list<std::string>::const_iterator it = _valuesNeeded.begin();
       it != _valuesNeeded.end(); it++) {
    out += "\n" + prefix + "     " + *it;
  }
  if (_valuesNeeded.empty()) out += "\n" + prefix + "     <none>";

  for (std::vector<VRDisplayNode*>::const_iterator it = _children.begin();
       it != _children.end(); it++) {
    out += "\n" + (*it)->printNode(prefix + "| ");
  }

  return out;
}


} /* namespace MinVR */
