/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GLFWWINDOW_H_
#define GLFWWINDOW_H_

#include "display/VRDisplayDevice.h"
#include <GLFW/glfw3.h>
#include "GlfwInputDevice.h"

namespace MinVR {

class GlfwWindow : public VRDisplayDevice {
public:
	GlfwWindow(int x, int y, int width, int height);
	virtual ~GlfwWindow();

	int getWidth();
	int getHeight();

	void initialize();
	bool isOpen();
	void useDisplay(const MinVR::VRDisplayAction& action);
	void finishRendering();

	GLFWwindow* getWindow() const {
		return window;
	}

	void setInputDevice(GlfwInputDevice* inputDevice) {
		this->inputDevice = inputDevice;
	}

	//void addSubDisplay(VRDisplayDevice* display);


protected:
	void startRendering(const MinVR::VRRenderer& renderer, int x);

private:
	GLFWwindow* window;
	int x, y, width, height;
	GlfwInputDevice* inputDevice;
	//std::vector<VRDisplayDevice*> subDisplays;
};

} /* namespace MinVR */

#endif /* GLFWWINDOW_H_ */
