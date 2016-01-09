/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GLFWWINDOW_H_
#define GLFWWINDOW_H_

#include "plugin/PluginFramework.h"
#include "display/concrete/BaseDisplayDevice.h"
#include <GLFW/glfw3.h>
#include "GlfwInputDevice.h"

namespace MinVR {

class GlfwWindow : public BaseDisplayDevice {
public:
	PLUGIN_API GlfwWindow(int x, int y, int width, int height);
	PLUGIN_API virtual ~GlfwWindow();

	PLUGIN_API int getWidth();
	PLUGIN_API int getHeight();

	PLUGIN_API void initialize();
	PLUGIN_API bool isOpen();
	PLUGIN_API void useDisplay(const MinVR::VRDisplayAction& action);
	PLUGIN_API void finishRendering();

	PLUGIN_API GLFWwindow* getWindow() const {
		return window;
	}

	PLUGIN_API void setInputDevice(GlfwInputDevice* inputDevice) {
		this->inputDevice = inputDevice;
	}

protected:
	PLUGIN_API void startRendering(const MinVR::VRRenderer& renderer, VRRenderState& state);

private:
	GLFWwindow* window;
	int x, y, width, height;
	GlfwInputDevice* inputDevice;
	//std::vector<VRDisplayDevice*> subDisplays;
};

} /* namespace MinVR */

#endif /* GLFWWINDOW_H_ */
