/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <plugin/VRPlugin.h>
#include <main/VRFactory.h>
#include "VRTUIODevice.h"


// special: include this only once in one .cpp file per plugin
#include <plugin/VRPluginVersion.h>


namespace MinVR {



class VRTUIOPlugin : public VRPlugin {
public:
	PLUGIN_API VRTUIOPlugin() {
      //std::cout << "VRTUIOPlugin created." << std::endl;
	}

	PLUGIN_API virtual ~VRTUIOPlugin() {
      //std::cout << "VRTUIOPlugin destroyed." << std::endl;
	}

	PLUGIN_API void registerWithMinVR(VRMainInterface *vrMain)
	{
      //std::cout << "Registering VRTUIOPlugin." << std::endl;
		vrMain->getFactory()->registerItemType<VRInputDevice, VRTUIODevice>("VRTUIODevice");
	}

	PLUGIN_API void unregisterWithMinVR(VRMainInterface *vrMain)
	{
      //std::cout << "Unregistering VRTUIOPlugin." << std::endl;
	}
};

} // end namespace


extern "C"
{
	PLUGIN_API MinVR::VRPlugin* createPlugin() {
		return new MinVR::VRTUIOPlugin();
	}
}

