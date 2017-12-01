/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRCONSOLENODE_H_
#define VRCONSOLENODE_H_

#include <iostream>

#include "VRDisplayNode.h"
#include <main/VRFactory.h>
#include <main/VRError.h>

namespace MinVR {

class VRConsoleNode : public VRDisplayNode {
public:
	VRConsoleNode(const std::string &name, std::ostream *stream = &std::cout);
	virtual ~VRConsoleNode();

	virtual std::string getType() const { return "VRConsoleNode"; }

	void render(VRDataIndex *renderState, VRRenderHandler* renderHandler);
	void displayFinishedRendering(VRDataIndex *renderState);

	/// Overridden here in order to generate an error - a console node cannot have children.
	void addChild(VRDisplayNode* child);

	void println(const std::string &output);

	static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

private:
	std::ostream* m_stream;
};

} /* namespace MinVR */

#endif /* VRCONSOLENODE_H_ */
