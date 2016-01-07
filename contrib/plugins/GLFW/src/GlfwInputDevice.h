/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GLFWINPUTDEVICE_H_
#define GLFWINPUTDEVICE_H_

#include "event/VRInputDevice.h"
#include <vector>
#include <GLFW/glfw3.h>

namespace MinVR {

class GlfwInputDevice : public VRInputDevice {
public:
	GlfwInputDevice();
	virtual ~GlfwInputDevice();

	void appendNewInputEventsSinceLastCall(VRDataQueue& queue);
	void registerGlfwWindow(GLFWwindow* window);

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
	std::vector<std::string> events;
	VRDataIndex dataIndex;
};

class GlfwInputDeviceFactory : public VRInputDeviceFactory {
public:
	GlfwInputDeviceFactory(VRInputDevice* inputDevice) : device(inputDevice) {}
	virtual ~GlfwInputDeviceFactory() {}

	std::vector<VRInputDevice*> create(const VRDataIndex& dataIndex);

private:
	VRInputDevice* device;
};

} /* namespace MinVR */

#endif /* GLFWINPUTDEVICE_H_ */
