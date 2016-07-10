/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Ben Knorlein
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <plugin/VRPlugin.h>
#include <display/VRWindowToolkit.h>
#include "VRFreeGLUTWindowToolkit.h"


// special: include this only once in one .cpp file per plugin
#include <plugin/VRPluginVersion.h>


namespace MinVR {



class VRFreeGLUTPlugin : public VRPlugin {
public:
	PLUGIN_API VRFreeGLUTPlugin() {
      //std::cout << "GlfwPlugin created." << std::endl;
	}

	PLUGIN_API virtual ~VRFreeGLUTPlugin() {
      //std::cout << "GlfwPlugin destroyed." << std::endl;
	}

	PLUGIN_API void registerWithMinVR(VRMainInterface *vrMain)
	{
      //std::cout << "Registering VRGLFWPlugin." << std::endl;
		vrMain->getFactory()->registerItemType<VRWindowToolkit, VRFreeGLUTWindowToolkit>("VRFreeGLUTWindowToolkit");
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
		return new MinVR::VRFreeGLUTPlugin();
	}
}

