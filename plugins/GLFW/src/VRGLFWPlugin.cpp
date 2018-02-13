/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

 // This avoids APIENTRY macro redefinition warnings when including glfw3.h
#if defined (WIN32)
#define NOMINMAX
#include <winsock2.h>
#include <windows.h>
#endif

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <plugin/VRPlugin.h>
#include <display/VRWindowToolkit.h>
#include "VRGLFWWindowToolkit.h"


// special: include this only once in one .cpp file per plugin
#include <plugin/VRPluginVersion.h>


namespace MinVR {



class VRGLFWPlugin : public VRPlugin {
public:
	PLUGIN_API VRGLFWPlugin() {
      //std::cout << "GlfwPlugin created." << std::endl;
	}

	PLUGIN_API virtual ~VRGLFWPlugin() {
      //std::cout << "GlfwPlugin destroyed." << std::endl;
	}

	PLUGIN_API void registerWithMinVR(VRMainInterface *vrMain)
	{
      //std::cout << "Registering VRGLFWPlugin." << std::endl;
		vrMain->getFactory()->registerItemType<VRWindowToolkit, VRGLFWWindowToolkit>("VRGLFWWindowToolkit");
	}

	PLUGIN_API void unregisterWithMinVR(VRMainInterface *vrMain)
	{
      //std::cout << "Unregistering GlfwPlugin." << std::endl;
		// TODO
	}
};

} // end namespace


extern "C"
{
	PLUGIN_API MinVR::VRPlugin* createPlugin() {
		return new MinVR::VRGLFWPlugin();
	}
}

