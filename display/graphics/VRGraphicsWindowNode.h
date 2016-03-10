/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRGRAPHICSWINDOWNODE_H_
#define VRGRAPHICSWINDOWNODE_H_

#include "display/base/VRDisplayNode.h"
#include "VRGraphicsWindowChild.h"
#include "display/synchronization/VRSynchronizedDisplay.h"

namespace MinVR {

class VRGraphicsWindowNode : public VRDisplayNode, VRSynchronizedDisplay {
public:
	virtual ~VRGraphicsWindowNode();

	virtual void render(VRRenderer& renderer);
	virtual void startRender(VRRenderer& renderer);
	virtual void waitForRenderComplete();
	virtual void synchronize();

	void addChild(VRGraphicsWindowChild* child);

protected:
	virtual void setCurrentContext() = 0;
	virtual void clearCurrentContext() = 0;
	virtual void swapBuffers() = 0;
	virtual void flush() = 0;
	virtual void finish() = 0;
};

} /* namespace MinVR */

#endif /* VRGRAPHICSWINDOWNODE_H_ */
