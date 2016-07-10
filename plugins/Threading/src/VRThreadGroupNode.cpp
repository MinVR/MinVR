/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <VRThreadGroupNode.h>
#include <iostream>

namespace MinVR {

VRThreadGroupNode::VRThreadGroupNode(const std::string& name) : VRDisplayNode(name), threadGroup(NULL) {
}

VRThreadGroupNode::~VRThreadGroupNode() {
	// Terminate render thread loop
	threadGroup->startThreadAction(THREADACTION_Terminate, NULL, NULL);

	// Delete render threads
	for (int f = 0; f < renderThreads.size(); f++) {
		delete renderThreads[f];
	}

	// Delete thread group
	if (threadGroup) {
		delete threadGroup;
	}
}

void VRThreadGroupNode::render(VRDataIndex* renderState,
		VRRenderHandler* renderHandler) {

	// If the threadGroup has not been created, create the render threads
	if (!threadGroup) {
		int numThreads = getChildren().size();
		threadGroup = new VRThreadGroup(numThreads);

		for (int f = 0; f < numThreads; f++) {
			renderThreads.push_back(new VRRenderThread(getChildren()[f], threadGroup));
		}
	}

	// Let threads know they are ready for rendering
	threadGroup->startThreadAction(THREADACTION_Render, renderState, renderHandler);
	threadGroup->waitForComplete();
}

void VRThreadGroupNode::waitForRenderToComplete(VRDataIndex* renderState) {
	// Let threads know we are waiting for them to finish rendering
	threadGroup->startThreadAction(THREADACTION_WaitForRenderToComplete, renderState, NULL);
	threadGroup->waitForComplete();
}

void VRThreadGroupNode::displayFinishedRendering(VRDataIndex* renderState) {
	// Let threads know that we should display the results
	threadGroup->startThreadAction(THREADACTION_DisplayFinishedRendering, renderState, NULL);
	threadGroup->waitForComplete();
}

VRDisplayNode* VRThreadGroupNode::create(VRMainInterface* vrMain,
		VRDataIndex* config, const std::string& nameSpace) {
	return new VRThreadGroupNode(nameSpace);
}

} /* namespace MinVR */
