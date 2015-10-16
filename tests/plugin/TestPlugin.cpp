/*
 * Copyright Regents of the University of Minnesota, 2014.  This software is released under the following license: http://opensource.org/licenses/lgpl-3.0.html.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "plugin/Plugin.h"
#include <iostream>
#include "GraphicsInterface.h"
#include "DeviceInterface.h"
#include "TestPluginDrivers.h"

class TestPlugin : public MinVR::Plugin {
public:
	PLUGIN_API TestPlugin() {
		std::cout << "TestPlugin created." << std::endl;
	}
	PLUGIN_API virtual ~TestPlugin() {
		std::cout << "TestPlugin destroyed." << std::endl;
	}
	PLUGIN_API bool registerPlugin(MinVR::PluginInterface *interface)
	{
		std::cout << "Registering TestPlugin with the following interface: " << interface->getName() << std::endl;
		if (interface->getName() == "GraphicsInterface")
		{
			GraphicsInterface* graphicsInterface = interface->getInterface<GraphicsInterface>();
			graphicsInterface->addGraphicsDriver("opengl", new OpenGLGraphicsDriver());
			graphicsInterface->addGraphicsDriver("d3d", new D3DGraphicsDriver());
			return true;
		}
		if (interface->getName() == "DeviceInterface")
		{
			DeviceInterface* deviceInterface = interface->getInterface<DeviceInterface>();
			deviceInterface->addInputDeviceFactory(new VRPNFactory());
			deviceInterface->addInputDeviceFactory(new TouchFactory());
			return true;
		}

		return false;
	}
	PLUGIN_API bool unregisterPlugin(MinVR::PluginInterface *interface)
	{
		std::cout << "Unregistering TestPlugin with the following interface: " << interface->getName() << std::endl;
		return false;
	}
};

extern "C"
{
	PLUGIN_API MinVR::Plugin* loadPlugin() {
		return new TestPlugin();
	}
}
