/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/base/VRDisplayNode.h>

using namespace std;

namespace MinVR {

VRDisplayNode::VRDisplayNode() {

}

VRDisplayNode::~VRDisplayNode() {
	for (vector<VRDisplay*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		delete (*it);
	}
}

void VRDisplayNode::render(VRRenderer& renderer) {
	for (vector<VRDisplay*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		(*it)->render(renderer);
	}
}

const std::vector<VRDisplay*>& VRDisplayNode::getChildren() const {
	return m_children;
}

void VRDisplayNode::addChild(VRDisplay* child) {
	m_children.push_back(child);
}

} /* namespace MinVR */
