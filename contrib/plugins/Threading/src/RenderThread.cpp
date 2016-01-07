/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "RenderThread.h"
#include <iostream>

namespace MinVR {

RenderThread::RenderThread(VRDisplayDevice* display, RenderThreadInfo* threadInfo)
	: display(display), threadInfo(threadInfo), frame(0) {
	// TODO Auto-generated constructor stub
	_thread = new Thread(&RenderThread::render, this);
}

RenderThread::~RenderThread() {
	// TODO Auto-generated destructor stub
	if (_thread) {
		_thread->join();
	}

	delete _thread;
}

void RenderThread::render() {
	threadInfo->barrier->wait();

	while (true)
	{

		RenderThreadAction action = THREADACTION_NONE;

		// Wait for the main thread to signal that it's ok to start rendering
		UniqueMutexLock startActionLock(threadInfo->startActionMutex);
		while (threadInfo->threadAction == THREADACTION_NONE) {
			threadInfo->startActionCond.wait(startActionLock);
		}
		if (threadInfo->threadAction == THREADACTION_TERMINATE) {
			// THREADACTION_TERMINATE is a special flag used to quit the application and cleanup all the threads nicely
			startActionLock.unlock();
			return;
		}

		action = threadInfo->threadAction;

		startActionLock.unlock();

		if (action == THREADACTION_RENDER)
		{
			frame++;
			//std::cout << "RENDER " << frame << std::endl << std::flush;
			VRDisplayDevice::startRendering(display, *(threadInfo->renderer), threadInfo->x);
		}
		else if (action == THREADACTION_ACTION)
		{
			//std::cout << "ACTION " << frame << std::endl << std::flush;
			threadInfo->action->exec();
		}

		threadInfo->startedActionMutex.lock();
		threadInfo->numThreadsStarted++;
		if (threadInfo->numThreadsStarted >= threadInfo->numThreads)
		{
			threadInfo->threadAction = THREADACTION_NONE;
		}
		threadInfo->startedActionCond.notify_all();
		threadInfo->startedActionMutex.unlock();

		threadInfo->barrier->wait();

		if (action == THREADACTION_RENDER)
		{
			display->finishRendering();
		}

		threadInfo->endActionMutex.lock();
		threadInfo->numThreadsCompleted++;
		threadInfo->endActionCond.notify_all();
		threadInfo->endActionMutex.unlock();

		threadInfo->barrier->wait();
	}
}

} /* namespace MinVR */
