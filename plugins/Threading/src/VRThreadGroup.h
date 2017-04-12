/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRTHREADGROUP_H_
#define VRTHREADGROUP_H_

#include "VRThread.h"
#include "display/VRDisplayNode.h"

namespace MinVR {

/**
 * VRRenderThreadAction enables notification of what type of action
 * is being called for the whole thread group
 */
enum VRRenderThreadAction {
	THREADACTION_None,
	THREADACTION_Init,
	THREADACTION_Render,
	THREADACTION_WaitForRenderToComplete,
	THREADACTION_DisplayFinishedRendering,
	THREADACTION_Terminate };

/**
 * VRThreadGroup is an object that keeps track of all the threading specific synchronization.  It also
 * has some helper methods which make it easier to synchronize actions between threads and the controller.
 */
class VRThreadGroup
{
public:
	/// Initialize with the number of threads
	VRThreadGroup(int numThreads);
	virtual ~VRThreadGroup();

	/// Acts a as synchronization point for all threads
	void synchronize();

	/// Starts an action on all the threads
	void startThreadAction(VRRenderThreadAction threadAction, VRDataIndex* state, VRRenderHandler* renderer);

	/// Waits for an action to be started
	VRRenderThreadAction waitForAction();

	/// Notifies the thread group that an action is completed
	void completeAction();

	/// Waits for all threads to be completed
	void waitForComplete();

	VRRenderHandler* getRenderHandler() const {
		return renderHandler;
	}

	void setRenderHandler(VRRenderHandler* renderHandler) {
		this->renderHandler = renderHandler;
	}

	VRDataIndex* getRenderState() const {
		return renderState;
	}

	void setRenderState(VRDataIndex* renderState) {
		this->renderState = renderState;
	}

	Mutex syncMutex;

private:
	VRRenderThreadAction threadAction;
	Barrier* barrier;
	int numThreads;
	Mutex actionMutex;
	ConditionVariable actionCond;
	Mutex actionCompletedMutex;
	ConditionVariable actionCompletedCond;
	int numThreadsCompleted;

	VRDataIndex* renderState;
	VRRenderHandler* renderHandler;
};

} /* namespace MinVR */

#endif /* VRTHREADGROUP2_H_ */
