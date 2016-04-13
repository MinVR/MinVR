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
	PLUGIN_API GlfwPlugin() {
		std::cout << "GlfwPlugin created." << std::endl;

	    glfwSetErrorCallback(error_callback);


	    // TODO:  Should Init happen here or when the first device is created?
	    if (!glfwInit()) {
	    	std::cout << "GLFW Init failed." << std::endl;
	    	exit(0);
	    }
	}

	PLUGIN_API virtual ~GlfwPlugin() {

	    // TODO:  Should Terminate happen here or when the last device is destroyed?
        glfwTerminate();

		std::cout << "GlfwPlugin destroyed." << std::endl;
	}

	PLUGIN_API bool registerWithMinVR(MinVR::VRMain *vrMain)
	{
		std::cout << "Registering VRGLFWPlugin." << std::endl;
		vrMain->getFactory()->addSubFactory(new VRGLFWGraphicsWindowNodeFactory());
		vrMain->getFactory()->addSubFactory(new VRGLFWInputDeviceFactory());
		return true;
	}

	PLUGIN_API bool unregisterWithMinVR(MinVR::VRMain *vrMain)
	{
		std::cout << "Unregistering GlfwPlugin." << std::endl;
		return true;
	}
};

} // end namespace


extern "C"
{
	PLUGIN_API MinVR::Plugin* createPlugin() {
		return new MinVR::VRGLFWPlugin();
	}
}

