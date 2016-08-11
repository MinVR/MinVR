/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRRENDERTHREAD_H_
#define VRRENDERTHREAD_H_

#include "VRThreadGroup.h"

namespace MinVR {

/**
 * VRRenderThread has a render method which controls the specific actions sent to the threads.
 * It also tells where to synchronize and wait for the other threads.
 */
class VRRenderThread {
public:
	/// Render threads render one display node and synchronize with other threads through the
	/// shared threadGroup object.
	VRRenderThread(VRDisplayNode* displayNode, VRThreadGroup* threadGroup);
	virtual ~VRRenderThread();

	/// The render method which controls actions.
	void render();

	int threadId;

private:
	VRDisplayNode* displayNode;
	VRThreadGroup* threadGroup;
	Thread* thread;
};

} /* namespace MinVR */

#endif /* VRRENDERTHREAD_H_ */
