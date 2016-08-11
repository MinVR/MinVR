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

VRThreadGroupNode::VRThreadGroupNode(const std::string& name, bool asyncEnabled) : VRDisplayNode(name), threadGroup(NULL), async(true), asyncEnabled(asyncEnabled) {
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
		async = asyncEnabled;
		int numThreads = getChildren().size();
		threadGroup = new VRThreadGroup(numThreads);

		for (int f = 0; f < numThreads; f++) {
			renderThreads.push_back(new VRRenderThread(getChildren()[f], threadGroup));
			renderThreads[f]->threadId = f;
		}
	}

	if (async) {
		// Let threads know they are ready for rendering
			VRDataIndex rstate;
			threadGroup->startThreadAction(THREADACTION_Render, &rstate, renderHandler);
			threadGroup->waitForComplete();
	}
	else {
		VRDisplayNode::render(renderState, renderHandler);
	}

}

void VRThreadGroupNode::waitForRenderToComplete(VRDataIndex* renderState) {
	if (async) {
		// Let threads know we are waiting for them to finish rendering
		VRDataIndex rstate;
		threadGroup->startThreadAction(THREADACTION_WaitForRenderToComplete, &rstate, NULL);
		threadGroup->waitForComplete();
	}
	else {
		VRDisplayNode::waitForRenderToComplete(renderState);
	}
}

void VRThreadGroupNode::displayFinishedRendering(VRDataIndex* renderState) {
	if (async) {
		// Let threads know that we should display the results
		VRDataIndex rstate;
		threadGroup->startThreadAction(THREADACTION_DisplayFinishedRendering, &rstate, NULL);
		threadGroup->waitForComplete();
	}
	else {
		VRDisplayNode::displayFinishedRendering(renderState);
	}

	async = asyncEnabled;
}

VRDisplayNode* VRThreadGroupNode::create(VRMainInterface* vrMain,
		VRDataIndex* config, const std::string& nameSpace) {
	bool asyncEnabled = int(config->getValue("AsyncEnabled", nameSpace));
	return new VRThreadGroupNode(nameSpace, asyncEnabled);
}

} /* namespace MinVR */
