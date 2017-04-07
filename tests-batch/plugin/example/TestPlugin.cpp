/* ================================================================================

This file is part of the MinVR Open Source Project.

File: TestPlugin.cpp

Original Author(s) of this File:
	Dan Orban, 2015, University of Minnesota

Author(s) of Significant Updates/Modifications to the File:
	...

-----------------------------------------------------------------------------------
Copyright (c) 2015 Regents of the University of Minnesota
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* The name of the University of Minnesota, nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================ */

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
	PLUGIN_API bool registerPlugin(MinVR::PluginInterface *iface)
	{
		std::cout << "Registering TestPlugin with the following interface: " << iface->getName() << std::endl;

		GraphicsInterface* graphicsInterface = iface->getInterface<GraphicsInterface>();
		if (graphicsInterface != NULL)
		{
			graphicsInterface->addGraphicsDriver("opengl", new OpenGLGraphicsDriver());
			graphicsInterface->addGraphicsDriver("d3d", new D3DGraphicsDriver());
			return true;
		}

		DeviceInterface* deviceInterface = iface->getInterface<DeviceInterface>();
		if (deviceInterface != NULL)
		{
			deviceInterface->addInputDeviceFactory(new VRPNFactory());
			deviceInterface->addInputDeviceFactory(new TouchFactory());
			return true;
		}

		return false;
	}
	PLUGIN_API bool unregisterPlugin(MinVR::PluginInterface *iface)
	{
		std::cout << "Unregistering TestPlugin with the following interface: " << iface->getName() << std::endl;

		return true;
	}
};

extern "C"
{
	PLUGIN_API MinVR::Plugin* loadPlugin() {
		return new TestPlugin();
	}
}
