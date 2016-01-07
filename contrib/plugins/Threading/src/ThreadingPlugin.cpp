/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "plugin/Plugin.h"
#include <iostream>
#include "main/VRPluginInterface.h"
#include "ThreadedDisplay.h"

namespace MinVR {

class ThreadingPlugin : public MinVR::Plugin {
public:
	PLUGIN_API ThreadingPlugin() {
		std::cout << "ThreadingPlugin created." << std::endl;
	}
	PLUGIN_API virtual ~ThreadingPlugin() {
		std::cout << "ThreadingPlugin destroyed." << std::endl;
	}
	PLUGIN_API bool registerPlugin(MinVR::PluginInterface *iface)
	{
		std::cout << "Registering ThreadingPlugin with the following interface: " << iface->getName() << std::endl;

		VRPluginInterface* vrInterface = iface->getInterface<VRPluginInterface>();
		if (vrInterface != NULL)
		{
			displayFactory = new ThreadedDisplayFactory();
			vrInterface->addVRDisplayDeviceFactory(displayFactory);
			return true;
		}

		return false;
	}
	PLUGIN_API bool unregisterPlugin(MinVR::PluginInterface *iface)
	{
		std::cout << "Unregistering ThreadingPlugin with the following interface: " << iface->getName() << std::endl;
		//delete displayFactory;
		return true;
	}

private:
	ThreadedDisplayFactory* displayFactory;
};

}

extern "C"
{
	PLUGIN_API MinVR::Plugin* loadPlugin() {
		return new MinVR::ThreadingPlugin();
	}
}

