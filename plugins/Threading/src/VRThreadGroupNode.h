/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRTHREADGROUPNODE_H_
#define VRTHREADGROUPNODE_H_

#include "VRRenderThread.h"
#include "display/VRDisplayNode.h"
#include <vector>

namespace MinVR {

/**
 * VRThreadGroupNode runs each of the child display nodes in their own thread.  It synchronizes the three main
 * display node functions render, waitForRenderToComplete, and displayFinishedRendering.  This allows multithreading
 * at the display node level, while enforcing that nodes are displayed at the same time.  It also allows for
 * the potential for other threads in MinVR to use the time between render and waitForRenderComplete.  It is possible
 * to nest the VRThreadGroupNodes enableing multi levels of parallel processing.
 */
class VRThreadGroupNode : public VRDisplayNode {
public:
	VRThreadGroupNode(const std::string &name, bool asyncEnabled);
	virtual ~VRThreadGroupNode();

	virtual std::string getType() const { return "VRThreadGroupNode"; }

	// The three main methods that need to be synchronized at the thread level
	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);
	virtual void waitForRenderToComplete(VRDataIndex *renderState);
	virtual void displayFinishedRendering(VRDataIndex *renderState);

	static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

private:
	std::vector<VRRenderThread*> renderThreads;
	VRThreadGroup* threadGroup;
	bool async;
	bool asyncEnabled;
};

} /* namespace MinVR */

#endif /* VRTHREADGROUPNODE_H_ */
