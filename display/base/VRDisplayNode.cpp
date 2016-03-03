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
	for (vector<VRDisplayGraphNode*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		delete (*it);
	}
}

void VRDisplayNode::startRender() {
	for (vector<VRDisplayGraphNode*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		(*it)->startRender();
	}
}

void VRDisplayNode::waitForRenderComplete() {
	for (vector<VRDisplayGraphNode*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		(*it)->waitForRenderComplete();
	}
}

void VRDisplayNode::synchronize() {
	for (vector<VRDisplayGraphNode*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		(*it)->synchronize();
	}
}

const std::vector<VRDisplayGraphNode*>& VRDisplayNode::getChildren() const {
	return m_children;
}

void VRDisplayNode::addChild(VRDisplayGraphNode* child) {
	m_children.push_back(child);
}

} /* namespace MinVR */
