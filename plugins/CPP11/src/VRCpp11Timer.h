/*
 * Copyright Regents of the University of Minnesota, 2017.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRCPP11TIMER_H_
#define VRCPP11TIMER_H_

#include <chrono>
#include "input/VRInputDevice.h"
#include "main/VRMainInterface.h"
#include "config/VRDataQueue.h"

namespace MinVR {

class VRCpp11Timer : public VRInputDevice {
public:
	VRCpp11Timer();
	virtual ~VRCpp11Timer();

	void appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents);

	static VRInputDevice* create(VRMainInterface *vrMain, VRDataIndex *config,const std::string &nameSpace);

private:
	double getTime();

	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	VRDataIndex dataIndex;
};

} /* namespace MinVR */

#endif /* VRCPP11TIMER_H_ */
