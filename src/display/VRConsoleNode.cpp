/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/VRConsoleNode.h>

namespace MinVR {

VRConsoleNode::VRConsoleNode(const std::string &name, std::ostream *stream) : m_stream(stream), VRDisplayNode(name) {
}

VRConsoleNode::~VRConsoleNode() {
}


void VRConsoleNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler) {

	rederState->pushState();
	renderState->setValue("IsConsole", 1);

	renderHandler->onVRRenderContext(renderState);
	renderHandler->onVRRenderScene(renderState);

	renderState->popState();
}

void VRConsoleNode::displayTheFinishedRendering() {
	std::flush(*m_stream);
}

void VRConsoleNode::println(const std::string &output) {
	*m_stream << output << std::endl;
}


VRDisplayNode*
VRConsoleNodeFactory::create(VRMain *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) {
	std::string nodeNameSpace = nameSpace + "/" + valName;

	std::string type = config->getValue("Type", nodeNameSpace);
	if (type != getType()) {
		// This factory cannot create the type specified
		return NULL;
	}

	std::ostream *stream
	if (config->getValue("Stream", nodeNameSpace) == "cerr") {
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


