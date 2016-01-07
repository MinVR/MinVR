/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef RENDERTHREAD_H_
#define RENDERTHREAD_H_

#include "display/VRDisplayDevice.h"
#include "Thread.h"

namespace MinVR {

enum RenderThreadAction { THREADACTION_NONE, THREADACTION_ACTION, THREADACTION_RENDER, THREADACTION_TERMINATE };

struct RenderThreadInfo
{
	Mutex startActionMutex;
	ConditionVariable startActionCond;
	Mutex startedActionMutex;
	ConditionVariable startedActionCond;
	Mutex endActionMutex;
	ConditionVariable endActionCond;
	Barrier* barrier;
	RenderThreadAction threadAction;
	int numThreadsStarted;
	int numThreadsCompleted;
	int numThreads;

	const VRDisplayAction* action;
	const VRRenderer* renderer;
	int x;
};

class RenderThread {
public:

	RenderThread(VRDisplayDevice* display, RenderThreadInfo* threadInfo);
	virtual ~RenderThread();

	void render();

private:
	VRDisplayDevice* display;
	Thread* _thread;

	RenderThreadInfo* threadInfo;

	int frame;
};

} /* namespace MinVR */

#endif /* RENDERTHREAD_H_ */
