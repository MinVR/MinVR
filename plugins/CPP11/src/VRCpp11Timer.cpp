/*
 * Copyright Regents of the University of Minnesota, 2017.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "VRCpp11Timer.h"

namespace MinVR {

VRCpp11Timer::VRCpp11Timer() {
	start = std::chrono::high_resolution_clock::now();
}

VRCpp11Timer::~VRCpp11Timer() {
}

VRInputDevice* VRCpp11Timer::create(VRMainInterface* vrMain,
		VRDataIndex* config, const std::string& nameSpace) {
	return new VRCpp11Timer();
}

double VRCpp11Timer::getTime() {
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end-start;
	return diff.count();
}

void VRCpp11Timer::appendNewInputEventsSinceLastCall(VRDataQueue* inputEvents) {
	std::string event = "FrameStart";
	std::string dataField = "/ElapsedSeconds";
	dataIndex.addData(event + dataField, getTime());
	inputEvents->push(dataIndex.serialize(event));
}

} /* namespace MinVR */
