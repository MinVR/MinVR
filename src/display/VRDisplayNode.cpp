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
		renderHandler->onVRRenderScene(*renderState);
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
      out[*it] = _name + " (" + getType() + ")";
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

std::string VRDisplayNode::printNode(std::set<std::string> valuesSet,
                                     const std::string &prefix) const {

  // First compress the long names a little bit for readability.
  std::string name;
  if (_name.size() > 48) {
    name = _name.substr(0,8) + "..." +
      _name.substr(_name.size() - 40, std::string::npos);

  } else {
    name = _name;
  }

  // Establish the output.
  std::string out;
  if (!prefix.empty()) out = "\n";

  // Display this node.
  out = prefix + "<displayNode:" + name + ">";

  // The lines following need a longer prefix.
  std::string newPrefix = prefix + " | ";

  out += "\n" + newPrefix + "  Type: " + getType();

  // Print the values added, but also add them to the set of values we
  // know from above this position in the display graph.
  out += "\n" + newPrefix + "  Values Added:";
  for (std::list<std::string>::const_iterator it = _valuesAdded.begin();
       it != _valuesAdded.end(); it++) {
    // Print the value added.
    out += "\n" + newPrefix + "    " + *it;

    // Insert it added value into the set from above.
    valuesSet.insert(*it);
  }
  if (_valuesAdded.empty()) out += "\n" + newPrefix + "     <none>";

  // Print the values needed, but also check them against the values already set.
  out += "\n" + newPrefix + "  Values Needed:";
  for (std::list<std::string>::const_iterator it = _valuesNeeded.begin();
       it != _valuesNeeded.end(); it++) {
    out += "\n" + newPrefix + "    " + *it;

    // If the name is not in the value set, flag it.
    if (valuesSet.find(*it) == valuesSet.end()) {

      out += " <-- ERROR: this value will not be found.";
    }
  }
  if (_valuesNeeded.empty()) out += "\n" + newPrefix + "     <none>";

  // Recursively print the children nodes.
  for (std::vector<VRDisplayNode*>::const_iterator it = _children.begin();
       it != _children.end(); it++) {
    out += "\n" + (*it)->printNode(valuesSet, newPrefix);
  }

  return out;
}


} /* namespace MinVR */
