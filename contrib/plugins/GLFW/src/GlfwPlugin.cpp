/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "plugin/Plugin.h"
#include <iostream>
#include "main/VRPluginInterface.h"
#include "GlfwWindowFactory.h"
#include "GlfwInputDevice.h"
#include "GlfwTimer.h"

namespace MinVR {

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

class GlfwPlugin : public MinVR::Plugin {
public:
	PLUGIN_API GlfwPlugin() : displayFactory(NULL), inputDeviceFactory(NULL), inputDevice(NULL), timer(NULL) {
		std::cout << "GlfwPlugin created." << std::endl;

	    glfwSetErrorCallback(error_callback);
	    if (!glfwInit()) {
	    	std::cout << "GLFW Init failed." << std::endl;
	    	exit(0);
	    }

		inputDevice = new GlfwInputDevice();
		displayFactory = new GlfwWindowFactory(inputDevice);
		inputDeviceFactory = new GlfwInputDeviceFactory(inputDevice);
		timer = new GlfwTimer();
	}
	PLUGIN_API virtual ~GlfwPlugin() {

		if (displayFactory != NULL)
		{
			delete displayFactory;
		}
		if (inputDeviceFactory != NULL)
		{
			delete inputDeviceFactory;
		}
		if (inputDevice != NULL)
		{
			delete inputDevice;
		}
		if (timer != NULL)
		{
			delete timer;
		}

        glfwTerminate();

		std::cout << "GlfwPlugin destroyed." << std::endl;
	}
	PLUGIN_API bool registerPlugin(MinVR::PluginInterface *iface)
	{
		std::cout << "Registering GlfwPlugin with the following interface: " << iface->getName() << std::endl;

		VRPluginInterface* vrInterface = iface->getInterface<VRPluginInterface>();
		if (vrInterface != NULL)
		{
			std::cout << "Adding GLFW window factory" << std::endl;
			vrInterface->addVRDisplayFactory(displayFactory);
			vrInterface->addVRInputDeviceFactory(inputDeviceFactory);
			vrInterface->addVRTimer(timer);
			return true;
		}

		return false;
	}
	PLUGIN_API bool unregisterPlugin(MinVR::PluginInterface *iface)
	{
		std::cout << "Unregistering GlfwPlugin with the following interface: " << iface->getName() << std::endl;

		return true;
	}

private:
	GlfwWindowFactory* displayFactory;
	GlfwInputDeviceFactory* inputDeviceFactory;
	GlfwTimer* timer;
	GlfwInputDevice* inputDevice;
};

}

extern "C"
{
	PLUGIN_API MinVR::Plugin* loadPlugin() {
		return new MinVR::GlfwPlugin();
	}
}

