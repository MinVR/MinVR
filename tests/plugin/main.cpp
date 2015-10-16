
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
