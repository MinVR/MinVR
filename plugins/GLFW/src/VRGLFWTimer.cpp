/*
 * Copyright Regents of the University of Minnesota, 2017.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <VRGLFWTimer.h>
#include <GLFW/glfw3.h>

namespace MinVR {

VRGLFWTimer::VRGLFWTimer() {
	// TODO Auto-generated constructor stub

}

VRGLFWTimer::~VRGLFWTimer() {
	// TODO Auto-generated destructor stub

}

void VRGLFWTimer::appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents) {
	std::string event = "FrameStart";
	std::string dataField = "/ElapsedSeconds";
	float time = glfwGetTime();
	dataIndex.addData(event + dataField, time);
	inputEvents->push(dataIndex.serialize(event));
}

VRInputDevice* VRGLFWTimer::create(VRMainInterface *vrMain, VRDataIndex *config,const std::string &nameSpace) {
	return new VRGLFWTimer();
}

} /* namespace MinVR */



