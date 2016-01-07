/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "ThreadedDisplay.h"
#include <iostream>

namespace MinVR {

int frame = 0;

ThreadedDisplay::ThreadedDisplay() : frame(0) {
}

ThreadedDisplay::~ThreadedDisplay() {
	for (int f = 0; f < renderThreads.size(); f++)
	{
		delete renderThreads[f];
	}
	delete threadInfo.barrier;
}

void ThreadedDisplay::useDisplay(const MinVR::VRDisplayAction& action) {
	threadInfo.numThreadsStarted = 0;
	threadInfo.numThreadsCompleted = 0;

	threadInfo.startActionMutex.lock();
	threadInfo.action = &action;
	threadInfo.threadAction = THREADACTION_ACTION;
	threadInfo.startActionCond.notify_all();
	threadInfo.startActionMutex.unlock();

	std::vector<VRDisplayDevice*> subDisplays = getSubDisplays();
	for (int f = 0; f < subDisplays.size(); f++)
	{
		if (!subDisplays[f]->allowThreading())
		{
			subDisplays[f]->use(action);
		}
	}

	// Wait for threads to finish rendering
	UniqueMutexLock endActionLock(threadInfo.endActionMutex);
	while (threadInfo.numThreadsCompleted < threadInfo.numThreads) {
		threadInfo.endActionCond.wait(endActionLock);
	}
	endActionLock.unlock();
}

void ThreadedDisplay::initialize() {
	std::vector<VRDisplayDevice*> threadedDisplays;
	for (int f = 0; f < getSubDisplays().size(); f++)
	{
		if (getSubDisplays()[f]->allowThreading())
		{
			threadedDisplays.push_back(getSubDisplays()[f]);
		}
	}

	threadInfo.threadAction = THREADACTION_NONE;
	threadInfo.numThreads = threadedDisplays.size();
	threadInfo.numThreadsStarted = 0;
	threadInfo.numThreadsCompleted = 0;
	threadInfo.barrier = new Barrier(threadInfo.numThreads);

	for (int f = 0; f < threadedDisplays.size(); f++)
	{
		renderThreads.push_back(new RenderThread(threadedDisplays[f], &threadInfo));
	}

	VRDisplayDevice::initialize();
}

void ThreadedDisplay::finishRendering() {
	//finishRenderingAllDisplays();


	// Wait for threads to finish rendering
	UniqueMutexLock endActionLock(threadInfo.endActionMutex);
	while (threadInfo.numThreadsCompleted < threadInfo.numThreads) {
		threadInfo.endActionCond.wait(endActionLock);
	}

	endActionLock.unlock();

	std::vector<VRDisplayDevice*> subDisplays = getSubDisplays();
	for (int f = 0; f < subDisplays.size(); f++)
	{
		if (!subDisplays[f]->allowThreading())
		{
			subDisplays[f]->finishRendering();
		}
	}
}

void ThreadedDisplay::startRendering(const MinVR::VRRenderer& renderer, int x) {
	frame++;

	threadInfo.numThreadsStarted = 0;
	threadInfo.numThreadsCompleted = 0;

	threadInfo.startActionMutex.lock();
	threadInfo.renderer = &renderer;
	threadInfo.x = x;
	threadInfo.threadAction = THREADACTION_RENDER;
	threadInfo.startActionCond.notify_all();
	threadInfo.startActionMutex.unlock();

	//std::cout << "start rendering threaded " << frame << std::endl << std::flush;
	std::vector<VRDisplayDevice*> subDisplays = getSubDisplays();
	for (int f = 0; f < subDisplays.size(); f++)
	{
		if (!subDisplays[f]->allowThreading())
		{
			VRDisplayDevice::startRendering(subDisplays[f], renderer, x);
		}
	}
	//startRenderingAllDisplays(renderer, x);

	UniqueMutexLock startedActionLock(threadInfo.startedActionMutex);
	while (threadInfo.numThreadsStarted < threadInfo.numThreads) {
		threadInfo.startedActionCond.wait(startedActionLock);
	}
	startedActionLock.unlock();
}

ThreadedDisplayFactory::ThreadedDisplayFactory() {
}

ThreadedDisplayFactory::~ThreadedDisplayFactory() {
}

VRDisplayDevice* ThreadedDisplayFactory::createDisplay(
		const std::string type, const std::string name, VRDataIndex& config,
		VRDisplayDeviceFactory* factory) {
	if (type == "thread_group")
	{
		return new ThreadedDisplay();
	}
	return NULL;
}

} /* namespace MinVR */


