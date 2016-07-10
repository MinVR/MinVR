/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <iostream>
#include <plugin/VRPlugin.h>
#include "VRThreadGroupNode.h"
#include "main/VRFactory.h"

// special: include this only once in one .cpp file per plugin
#include <plugin/VRPluginVersion.h>

namespace MinVR {

/** Threading Plugin for MinVR System.  Relies on c++11 threading or Boost for implementation.
 */
class VRThreadingPlugin : public MinVR::VRPlugin {
public:
	PLUGIN_API VRThreadingPlugin() {
	}

	PLUGIN_API virtual ~VRThreadingPlugin() {
	}

	PLUGIN_API void registerWithMinVR(VRMainInterface *vrMain)
	{
		// Register VRThreadGroupNode
		vrMain->getFactory()->registerItemType<VRDisplayNode, VRThreadGroupNode>("VRThreadGroupNode");
	}

	PLUGIN_API void unregisterWithMinVR(VRMainInterface *vrMain)
	{
	}

};

}

extern "C"
{
	PLUGIN_API MinVR::VRPlugin* createPlugin() {
		return new MinVR::VRThreadingPlugin();
	}
}

