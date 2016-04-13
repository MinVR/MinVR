/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/VRDisplayNode.h>

namespace MinVR {

VRDisplayNode::VRDisplayNode(const std::string &name) : _name(name) {

}

VRDisplayNode::~VRDisplayNode() {
	clearChildren(true);
}

void VRDisplayNode::render(VRDataIndex *renderState, VRRenderHandler* renderHandler) {
	if (m_children.size() > 0) {
		for (vector<VRDisplayNode*>::iterator it = m_children.begin(); it != m_children.end(); it++) {
			(*it)->render(renderState, renderHandler);
		}
	}
}

void VRDisplayNode::waitForRenderToComplete(VRDataIndex *renderState) {
	for (vector<VRDisplayNode*>::iterator it = m_children.begin(); it != m_children.end(); it++) {
		(*it)->waitForRenderToComplete(renderState);
	}
}

void VRDisplayNode::displayTheFinishedRendering(VRDataIndex *renderState) {
	for (vector<VRDisplayNode*>::iterator it = m_children.begin(); it != m_children.end(); it++) {
		(*it)->displayTheFinishedRendering(renderState);
	}
}

const std::vector<VRDisplayNode*>& VRDisplayNode::getChildren() const {
	return m_children;
}

void VRDisplayNode::addChild(VRDisplayNode* child) {
	m_children.push_back(child);
}

void VRDisplayNode::clearChildren(bool destroyChildren) {
	if (destroyChildren) {
		for (vector<VRDisplayNode*>::iterator it = m_children.begin(); it != m_children.end(); it++) {
				delete (*it);
		}
	}

	m_children.clear();
}

} /* namespace MinVR */
