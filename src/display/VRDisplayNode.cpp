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

void VRDisplayNode::auditValues(std::set<std::string> valuesSet,
                                const std::string &treeData) {

  //std::cerr << " checking: " << _name << std::endl;

  // Add the values supplied by this node to the set of values against which we
  // will check the values needed by this node.
  for (std::list<std::string>::const_iterator it = _valuesAdded.begin();
       it != _valuesAdded.end(); it++) {

    //std:: cerr << "    adding: " << *it << std::endl;

    // Insert it added value into the set from above.
    valuesSet.insert(*it);
  }

  // Run through all the valuesNeeded and see if they are in valuesSet.
  for (std::list<std::pair<std::string, bool> >::const_iterator it = _valuesNeeded.begin();
       it != _valuesNeeded.end(); it++) {

    //std:: cerr << "    needing: " << it->first << std::endl;

    // If the name is not in the value set, print the treeData and throw an error.
    if (valuesSet.find(it->first) == valuesSet.end()) {

      std::cerr << "Missing value in display node tree:" << std::endl
                << treeData << std::endl;

      // We only bomb if the value is required.
      if (it->second) {
        VRERROR("Necessary data (" + it->first + ") is not available to the " + getName() + " (" + getType() + ").",
                "Review the construction of the display node tree in your configuration file.");
      } else {
        VRWARNING("Optional data (" + it->first + ") is not available to the " + getName() + " (" + getType() + ").",
                  "Review the construction of the display node tree in your configuration file.");
      }
    }
  }

  // Recursively audit the children nodes.
  for (std::vector<VRDisplayNode*>::const_iterator it = _children.begin();
       it != _children.end(); it++) {
    (*it)->auditValues(valuesSet, treeData);
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
  for (std::list<std::pair<std::string, bool> >::const_iterator it = _valuesNeeded.begin();
       it != _valuesNeeded.end(); it++) {
    out += "\n" + newPrefix + "    " + it->first;
    if (it->second) {
      out += " (required)";
    } else {
      out += " (optional)";
    }

    // If the name is not in the value set, flag it.
    if (valuesSet.find(it->first) == valuesSet.end()) {

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
