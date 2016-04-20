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
}

VRConsoleNode::~VRConsoleNode() {
}


void VRConsoleNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler) {

  //renderState->pushState();
	renderState->addData("IsConsole", 1);

	renderHandler->onVRRenderContext(renderState, this);
	renderHandler->onVRRenderScene(renderState, this);

  //renderState->popState();
}

void VRConsoleNode::displayFinishedRendering(VRDataIndex *renderState) {
	std::flush(*m_stream);
}

void VRConsoleNode::println(const std::string &output) {
	*m_stream << output << std::endl;
}


VRDisplayNode*
VRConsoleNodeFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) {
	std::string nodeNameSpace = nameSpace + "/" + valName;

	std::string type = config->getValue("Type", nodeNameSpace);
	if (type != "VRConsoleNode") {
		// This factory cannot create the type specified
		return NULL;
	}

    std::ostream *stream;
    if (std::string(config->getValue("Stream", nodeNameSpace)) == "cerr") {
		stream = &std::cerr;
	}
	else {
		stream = &std::cout;
	}

	VRDisplayNode *node = new VRConsoleNode(valName, stream);

	// nothing more to do, no children for a console node.

	return node;
}


} /* namespace MinVR */


