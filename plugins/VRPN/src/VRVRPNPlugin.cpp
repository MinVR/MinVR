/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <iostream>
#include <plugin/VRPlugin.h>
#include "VRVRPNAnalogDevice.h"
#include "VRVRPNButtonDevice.h"
#include "VRVRPNTrackerDevice.h"

// special: include this only once in one .cpp file per plugin
#include <plugin/VRPluginVersion.h>


namespace MinVR {

class VRVRPNPlugin : public VRPlugin {
public:
	PLUGIN_API VRVRPNPlugin() {
		//std::cout << "VrpnPlugin created." << std::endl;
	}

	PLUGIN_API virtual ~VRVRPNPlugin() {
		//std::cout << "VrpnPlugin destroyed." << std::endl;
	}

	PLUGIN_API void registerWithMinVR(VRMainInterface *vrMain)
	{
        //std::cout << "Registering VRVRPNPlugin." << std::endl;
		vrMain->getFactory()->registerItemType<VRInputDevice, VRVRPNAnalogDevice>("VRVRPNAnalogDevice");
		vrMain->getFactory()->registerItemType<VRInputDevice, VRVRPNButtonDevice>("VRVRPNButtonDevice");
		vrMain->getFactory()->registerItemType<VRInputDevice, VRVRPNTrackerDevice>("VRVRPNTrackerDevice");
	}

	PLUGIN_API void unregisterWithMinVR(VRMainInterface *vrMain)
	{
        //std::cout << "Unregistering VRVRPNlugin." << std::endl;
		// TODO
	}
};

} // end namespace

extern "C"
{
	PLUGIN_API MinVR::VRPlugin* createPlugin() {
		return new MinVR::VRVRPNPlugin();
	}
}

