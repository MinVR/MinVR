/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Ben Knorlein
 */

#include <iostream>
#include <plugin/VRPlugin.h>
#include "VROpenVRNode.h"


// special: include this only once in one .cpp file per plugin
#include <plugin/VRPluginVersion.h>


namespace MinVR {

class VROpenVRPlugin : public VRPlugin {
public:
	PLUGIN_API VROpenVRPlugin() {
      //std::cout << "VRScalablePlugin created." << std::endl;
	}
	PLUGIN_API virtual ~VROpenVRPlugin() {
      //std::cout << "VRScalablePlugin destroyed." << std::endl;
	}

	PLUGIN_API void registerWithMinVR(VRMainInterface *vrMain)
	{
		//std::cout << "Registering VRScalablePlugin." << std::endl;
		vrMain->getFactory()->registerItemType<VRDisplayNode, VROpenVRNode>("VROpenVRNode");
	}

	PLUGIN_API void unregisterWithMinVR(VRMainInterface *vrMain)
	{
      //std::cout << "Unregistering VRScalablePlugin." << std::endl;
		// TODO
	}
};

} // end namespace

extern "C"
{
	PLUGIN_API MinVR::VRPlugin* createPlugin() {
		return new MinVR::VROpenVRPlugin();
	}
}

