/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <VRRenderThread.h>

namespace MinVR {

VRRenderThread::VRRenderThread(VRDisplayNode* displayNode,
		VRThreadGroup* threadGroup) : displayNode(displayNode), threadGroup(threadGroup) {

	// Start the render thread
	thread = new Thread(&VRRenderThread::render, this);
}

VRRenderThread::~VRRenderThread() {
	// Join the thread if needed
	if (thread) {
		thread->join();
	}

	// Delete thread
	delete thread;
}


void VRRenderThread::render() {
	// Ensure all threads are synchronized before starting to render
	threadGroup->synchronize();

	while (true) {
		// Wait for an action
		VRRenderThreadAction action = threadGroup->waitForAction();

		// If the action is terminate, exit loop
		if (action == THREADACTION_Terminate) {
			return;
		}
		else
		{
			// Copy render state from shared render state.  The copy is necessary for
			// display nodes to edit their own values
			VRDataIndex index;

			// Various render actions for the display nodes
			if (action == THREADACTION_Init) {
				// If the thread node is being initialized
				index.addData("/InitRender",1);
				displayNode->render(&index, threadGroup->getRenderHandler());
			}
			else if (action == THREADACTION_Render) {
				index.addData("/InitRender",0);
				displayNode->render(&index, threadGroup->getRenderHandler());
			}
			else if (action == THREADACTION_WaitForRenderToComplete) {
				displayNode->waitForRenderToComplete(&index);
			}
			else if (action == THREADACTION_DisplayFinishedRendering) {
				displayNode->displayFinishedRendering(&index);
			}
		}

		// Notify the thread group that this thread is complete
		threadGroup->completeAction();

		// Synchronize all threads here
		threadGroup->synchronize();
	}
}



} /* namespace MinVR */

