/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <VRThreadGroup.h>

namespace MinVR {

VRThreadGroup::VRThreadGroup(int numThreads) : numThreads(numThreads), threadAction(THREADACTION_None) {
	// Creates a barrier used for thread synchronization
	barrier = new Barrier(numThreads);
}

VRThreadGroup::~VRThreadGroup() {
}

VRRenderThreadAction VRThreadGroup::waitForAction() {
	// Loops until threadAction is not None
	UniqueMutexLock actionLock(actionMutex);
	while (threadAction == THREADACTION_None) {
		actionCond.wait(actionLock);
	}

	// Returns the defined action
	VRRenderThreadAction action = threadAction;

	actionLock.unlock();

	return action;
}

void VRThreadGroup::synchronize() {
	// synchronizes threads
	barrier->wait();
}

void VRThreadGroup::completeAction() {
	// Loops until threads are completed and sets the action back to None
	// once they are finished
	actionCompletedMutex.lock();
	numThreadsCompleted++;
	if (numThreadsCompleted == numThreads)
	{
		threadAction = THREADACTION_None;
	}
	actionCompletedCond.notify_all();
	actionCompletedMutex.unlock();
}

void VRThreadGroup::waitForComplete() {
	// Waits until all the threads are completed
	UniqueMutexLock completedActionLock(actionCompletedMutex);
	while (numThreadsCompleted < numThreads) {
		actionCompletedCond.wait(completedActionLock);
	}
	completedActionLock.unlock();
}

void VRThreadGroup::startThreadAction(VRRenderThreadAction threadAction, VRDataIndex* state, VRRenderHandler* renderer) {
	// Sets the approriate variables needed to run an action. Then it notifies all
	// the threads to wake up and perform that action.
	actionMutex.lock();
	numThreadsCompleted = 0;
	renderState = state;
	renderHandler = renderer;
	this->threadAction = threadAction;
	actionCond.notify_all();
	actionMutex.unlock();
}

} /* namespace MinVR */
