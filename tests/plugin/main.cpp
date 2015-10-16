
#include <iostream>
#include "plugin/PluginManager.h"
#include <vector>
#include "GraphicsInterface.h"

using namespace MinVR;

std::vector<GraphicsDriver*> graphicsDrivers;

class App : public GraphicsInterface
{
public:
	void addGraphicsDriver(std::string name, GraphicsDriver* driver)
	{
		graphicsDrivers.push_back(driver);
	}
};

int main(int argc, char **argv) {
  std::cout << "Registering plugins..." << std::endl;

  App app;

  PluginManager pluginManager;
  pluginManager.addInterface(&app);

  for (int f = 0; f < graphicsDrivers.size(); f++)
  {
	  delete graphicsDrivers[f];
  }
}

