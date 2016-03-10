/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRSYNCHRONIZEDDISPLAY_H_
#define VRSYNCHRONIZEDDISPLAY_H_

namespace MinVR {

class VRSynchronizedDisplay {
public:
	virtual ~VRSynchronizedDisplay() {}

	virtual void startRender() = 0;
	virtual void waitForRenderComplete() = 0;
	virtual void synchronize() = 0;
};

} /* namespace MinVR */

#endif /* ALLOWSTHREADING_H_ */
