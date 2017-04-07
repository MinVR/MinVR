/* ================================================================================

This file is part of the MinVR Open Source Project.

File: main.cpp

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

#include <iostream>
#include "plugin/PluginManager.h"
#include <vector>
#include "GraphicsInterface.h"
#include "DeviceInterface.h"

using namespace MinVR;
using namespace std;

void createDevice(string type, string parameters);

std::vector<GraphicsDriver*> graphicsDrivers;
std::vector<InputDeviceFactory*> deviceFactories;

class App : public GraphicsInterface, public DeviceInterface
{
public:
	void addGraphicsDriver(string name, GraphicsDriver* driver)
	{
		graphicsDrivers.push_back(driver);
	}
	void addInputDeviceFactory(InputDeviceFactory* factory)
	{
		deviceFactories.push_back(factory);
	}
};

int main(int argc, char **argv) {
  cout << "Registering plugins..." << endl;
  cout << "Plugin path: " << PLUGINPATH << endl;

  App app;

  PluginManager pluginManager;
  pluginManager.addInterface(dynamic_cast<GraphicsInterface*>(&app));
  pluginManager.addInterface(dynamic_cast<DeviceInterface*>(&app));
  pluginManager.loadPlugin(PLUGINPATH, "test_plugin");

  for (int f = 0; f < graphicsDrivers.size(); f++)
  {
	  graphicsDrivers[f]->draw();
  }

  createDevice("VRPNButton", "1");
  createDevice("VRPNButton", "2");
  createDevice("VRPNButton", "3");
  createDevice("VRPNTracker", "head");
  createDevice("TUIO", "3333");
  createDevice("Other", "");
  createDevice("Other2", "");
  createDevice("VRPNButton", "");

  // cleanup drivers:
  for (int f = 0; f < graphicsDrivers.size(); f++)
  {
	  delete graphicsDrivers[f];
  }
  for (int f = 0; f < deviceFactories.size(); f++)
  {
	  delete deviceFactories[f];
  }
}

void createDevice(string type, string parameters)
{
	for (int f = 0; f < deviceFactories.size(); f++)
	{
		if (deviceFactories[f]->createDevice(type, parameters))
		{
			return;
		}
	}

	cout << "Could not find device type: " << type << endl;
}
