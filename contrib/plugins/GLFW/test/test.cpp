#include <iostream>
#include <vector>
#include <string>
#include "plugin/PluginManager.h"
#include "plugin/PluginInterface.h"
#include "main/VRPluginInterface.h"
#include "main/VRTimer.h"
#include "display/concrete/CompositeDisplay.h"
#include "display/concrete/CompositeDisplayFactory.h"
#include <sstream>
#include <fstream>
#include "OpenGLGraphics.h"
using namespace MinVR;
using namespace std;

/*
 * Display and device factories retrieved from the interface
 */
std::vector<VRDisplayDeviceFactory*> displayFactories;
VRInputDeviceFactory* inputDeviceFactory;
VRTimer* mainTimer;

/*
 * Simple interface for interfacing with MinVR plugins.  It inherits
 * from VRPluginInterface which defines methods that plugins look for.
 * It is possible to define custom interfaces as well, but this interface
 * is specific for the base functionality in MinVR.
 */
class SimpleInterface : public MinVR::VRPluginInterface {
public:
	SimpleInterface() {}
	virtual ~SimpleInterface() {}

	// Adds the display factories for all plugins who use this interface
	void addVRDisplayDeviceFactory(VRDisplayDeviceFactory* factory) {displayFactories.push_back(factory);}
	// Adds the input device factories for all plugins who use this interface
	void addVRInputDeviceFactory(VRInputDeviceFactory* factory) { inputDeviceFactory = factory; }
	// Used for timing (i.e. for animation, etc...)
	void addVRTimer(VRTimer* timer) { mainTimer = timer; }
};

/*
 * Render and update methods
 */
void update();
void renderTriangle(VRRenderState& state);

/*
 * Main functionality
 */
int main(int argc, char **argv) {
  cout << "Registering plugins..." << endl;
  cout << "Plugin path: " << PLUGINPATH << endl;

  // Declare and initialize interface
  SimpleInterface iface;

  // Create plugin manager and add the MinVR interface
  PluginManager pluginManager;
  pluginManager.addInterface(dynamic_cast<SimpleInterface*>(&iface));

    string buildType = "";
#ifdef MinVR_DEBUG
    buildType = "d";
#endif
    
  // Load specific plugins which will initialize and add factories.
  // This can be defined inside the configuration itself
  pluginManager.loadPlugin(std::string(PLUGINPATH) + "/MinVR_GLFW", "MinVR_GLFW" + buildType);
  pluginManager.loadPlugin(std::string(PLUGINPATH) + "/MinVR_OpenGL", "MinVR_OpenGL" + buildType);
  pluginManager.loadPlugin(std::string(PLUGINPATH) + "/MinVR_Threading", "MinVR_Threading" + buildType);

  // Load configuration from file
  VRDataIndex config;
  std::string fileName = argv[1];
  config.processXMLFile(fileName, "");

  // Combine display factories into one factory for simplicity
  CompositeDisplayFactory factory(displayFactories);

  // Created the display from the factory (the display is composite,
  // so it contains multiple displays, but acts like one display)
  CompositeDisplay display(config, "VRDisplayDevices", &factory);
  display.initialize();

  // Create input device from factory (in this case only glfw keyboard / mouse)
  VRInputDevice* inputDevice = inputDeviceFactory->create(config)[0];

  // Create dataIndex and dataQueue
  VRDataQueue dataQueue;
  VRDataIndex dataIndex;

  bool isRunning = true;

  // Loop until escape key is hit or main display is closed
  while (display.isOpen() && isRunning)
  {
	  // Loop through new events
	  inputDevice->appendNewInputEventsSinceLastCall(dataQueue);
	  while (dataQueue.notEmpty())
	  {
		  // If escape is pressed, exit program
		  std::string p = dataIndex.addSerializedValue( dataQueue.getSerializedObject() );
		  if (p == "/keyboard")
		  {
			  std::string val = dataIndex.getValue("value", p);
			  if (val == "ESC_down")
			  {
				  isRunning = false;
			  }
			  cout << val << endl;
		  }
		  dataQueue.pop();
	  }

	  // Run a function on all contexts (not necessarily displays)
	  display.use(update);

	  // Render the triangle on all displays (passing render function into display)
	  // Includes viewports, threading, stereo displays, and custom display types
	  display.render(renderTriangle);
  }
}

/*
 * Update vertex arrays, etc...
 */
void update()
{
	// Example update of a vertex array
	GLfloat vertices[] = {0,0,0,1,1,1,0,0,0};
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDisableClientState(GL_VERTEX_ARRAY);
}

/*
 * Renders a triangle
 */
void renderTriangle(VRRenderState& state)
{
	  float ratio;
	  int width = 640;
	  int height = 480;
	  ratio = width / (float) height;
	  //glViewport(0, 0, width, height);
	  glClear(GL_COLOR_BUFFER_BIT);
	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();
	  glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	  glMatrixMode(GL_MODELVIEW);
	  glLoadIdentity();
	  glRotatef((float) mainTimer->getTime() * 50.f, 0.f, 0.f, 1.f);
	  glBegin(GL_TRIANGLES);
	  glColor3f(1.f, 0.f, 0.f);
	  glVertex3f(-0.6f, -0.4f, 0.f);
	  glColor3f(0.f, 1.f, 0.f);
	  glVertex3f(0.6f, -0.4f, 0.f);
	  glColor3f(0.f, 0.f, 1.f);
	  glVertex3f(0.f, 0.6f, 0.f);
	  glEnd();
}
