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
#include "GLUTWindow.h"

namespace MinVR {

class GLUTPlugin : public MinVR::Plugin {
public:
	PLUGIN_API GLUTPlugin() : displayFactory(NULL) {
		std::cout << "OpenGLPlugin created." << std::endl;
	}
	PLUGIN_API virtual ~GLUTPlugin() {
		std::cout << "OpenGLPlugin destroyed." << std::endl;
	}
	PLUGIN_API bool registerPlugin(MinVR::PluginInterface *iface)
	{
		std::cout << "Registering GLUTPlugin with the following interface: " << iface->getName() << std::endl;

		VRPluginInterface* vrInterface = iface->getInterface<VRPluginInterface>();
		if (vrInterface != NULL)
		{
			displayFactory = new GLUTWindowFactory();
			vrInterface->addVRDisplayDeviceFactory(displayFactory);
			return true;
		}

		return false;
	}
	PLUGIN_API bool unregisterPlugin(MinVR::PluginInterface *iface)
	{
		std::cout << "Unregistering GLUTPlugin with the following interface: " << iface->getName() << std::endl;
		delete displayFactory;
		return true;
	}

private:
	GLUTWindowFactory* displayFactory;
};

}

extern "C"
{
	PLUGIN_API MinVR::Plugin* loadPlugin() {
		return new MinVR::GLUTPlugin();
	}
}

