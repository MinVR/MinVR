/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/VRConsoleNode.h>

namespace MinVR {

VRConsoleNode::VRConsoleNode(const std::string &name, std::ostream *stream) : VRDisplayNode(name), m_stream(stream)  {
  _valuesAdded.push_back("/IsConsole");
}

VRConsoleNode::~VRConsoleNode() {
}

void VRConsoleNode::addChild(VRDisplayNode *child) {
  std::cerr << "Cannot add a child to a console node" << std::endl;
}
  
  
void VRConsoleNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler) {

  renderState->pushState();
	renderState->addData("/IsConsole", 1);

	renderHandler->onVRRenderContext(*renderState);
	renderHandler->onVRRenderScene(*renderState);

   renderState->popState();
}

void VRConsoleNode::displayFinishedRendering(VRDataIndex *renderState) {
	std::flush(*m_stream);
}

void VRConsoleNode::println(const std::string &output) {
	*m_stream << output << std::endl;
}


VRDisplayNode*
VRConsoleNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
	std::string nodeNameSpace = nameSpace;

    std::ostream *stream;
	if (std::string(config->getValue("Stream", nameSpace)) == "cerr") {
		stream = &std::cerr;
	}
	else {
		stream = &std::cout;
	}

	VRDisplayNode *node = new VRConsoleNode(nameSpace, stream);

	// nothing more to do, no children for a console node.

	return node;
}


} /* namespace MinVR */


