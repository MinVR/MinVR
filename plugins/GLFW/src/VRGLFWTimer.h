/*
 * Copyright Regents of the University of Minnesota, 2017.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRGLFWTIMER_H_
#define VRGLFWTIMER_H_

#include <VRGLFWInputDevice.h>
#include "main/VRMainInterface.h"
#include "config/VRDataQueue.h"

namespace MinVR {

class VRGLFWTimer : public VRInputDevice {
public:
	VRGLFWTimer();
	virtual ~VRGLFWTimer();

	void appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents);

	static VRInputDevice* create(VRMainInterface *vrMain, VRDataIndex *config,const std::string &nameSpace);

private:
	VRDataIndex dataIndex;
};

} /* namespace MinVR */

#endif /* VRGLFWTIMER_H_ */
