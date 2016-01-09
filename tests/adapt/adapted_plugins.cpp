#include <iostream>
#include <vector>
#include <string>
#include "plugin/PluginManager.h"
#include "plugin/PluginInterface.h"
#include "main/VRPluginInterface.h"
#include "main/VRTimer.h"
#include "display/CompositeDisplay.h"
#include <sstream>
#include <fstream>
#include <math.h>
#include "main/VRMain.h"

#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#endif

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
void initGL();
void render();
void reshape();
void eventCB(const std::string &eventName, VRDataIndex *dataIndex);
void renderCB(VRDataIndex* index);
void swapCB();

VRDisplayDevice* display;
VRMain *MVR;

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
  std::string fileName = argv[2];
  config.processXMLFile(fileName, "");

  // Combine display factories into one factory for simplicity
  CompositeDisplayFactory factory(displayFactories);

  // Created the display from the factory (the display is composite,
  // so it contains multiple displays, but acts like one display)
  display = new CompositeDisplay(config, "VRDisplayDevices", &factory);
  display->initialize();

  // Create input device from factory (in this case only glfw keyboard / mouse)
  VRInputDevice* inputDevice = inputDeviceFactory->create(config)[0];

  // Create dataIndex and dataQueue
  VRDataQueue dataQueue;
  VRDataIndex dataIndex;

  display->use(initGL);
  display->use(reshape);

  MVR = new VRMain();
  if (argc > 1) {
	  MVR->initialize(argv[1],argv[2]);
  }
  MVR->registerEventCallback(&eventCB);
  MVR->registerRenderCallback(&renderCB);
  MVR->registerSwapCallback(&swapCB);

  bool isRunning = true;

  // Loop until escape key is hit or main display is closed
  while (display->isOpen() && isRunning)
  {
	  // Loop through new events

	  MVR->synchronizeAndProcessEvents();
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

	  // Render the triangle on all displays (passing render function into display)
	  // Includes viewports, threading, stereo displays, and custom display types
	  display->startRendering(render);
	  MVR->renderEverywhere();
	  display->finishRendering();
  }

  delete display;
  delete MVR;
}


//-----------------------------------

double cameraPos[3] = {0.0f, 0.0f, 15.0f};
double targetPos[3] = {0.0f, 0.0f, 0.0f};
double cameraAim[3] = {0.0f, 1.0f, 0.0f};
double radius = 15.0;
double incAngle = -0.1f;
double horizAngle = 0.0;
double vertAngle = 0.0;

/* Initialize OpenGL Graphics */
void initGL() {
  //glutInitDisplayMode (GLUT_DEPTH);
  // Set background color to black and opaque
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  // Set background depth to farthest
  glClearDepth(1.0f);
  // Enable depth testing for z-culling
  glEnable(GL_DEPTH_TEST);
  // Set the type of depth-test
  glDepthFunc(GL_LEQUAL);
  // Enable smooth shading
  glShadeModel(GL_SMOOTH);
  // Nice perspective corrections
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void render() {
	GLfloat width = 640;
	GLfloat height = 480;

   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;

   // Set the viewport to cover the new window
   //glViewport(0, 0, (GLsizei)width, (GLsizei)height);

   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset

   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(1.6*45.0f, aspect, 0.1f, 100.0f);

   //The .m is the GLfloat* you are accessing
   //glMultMatrix( GLKMatrix4MakePerspective(45.0f, aspect, 0.1f,100.0f ).m );

	// Clear color and depth buffers
	   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	   // Enable depth testing for z-culling
	   glEnable(GL_DEPTH_TEST);
	   // Set the type of depth-test
	   glDepthFunc(GL_LEQUAL);

	   glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix

	   // Render a color-cube consisting of 6 quads with different colors
	   glLoadIdentity();                 // Reset the model-view matrix

	   // The stuff that used to be here has moved to the event handler.

	   gluLookAt (cameraPos[0], cameraPos[1], cameraPos[2],
	              targetPos[0], targetPos[1], targetPos[2],
	              cameraAim[0], cameraAim[1], cameraAim[2]);

	   glBegin(GL_LINES);
	     glColor3f(1.0, 1.0, 0.0);
	     glVertex3f(8.0, 0.0, 0.0);
	     glVertex3f(0.0, 0.0, 0.0);

	     glColor3f(0.0, 1.0, 1.0);
	     glVertex3f(0.0, 8.0, 0.0);
	     glVertex3f(0.0, 0.0, 0.0);

	     glColor3f(1.0, 0.0, 1.0);
	     glVertex3f(0.0, 0.0, 8.0);
	     glVertex3f(0.0, 0.0, 0.0);

	   glEnd();


	   glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
	      // Top face (y = 1.0f)
	      // Define vertices in counter-clockwise (CCW) order with normal
	      // pointing out
	     glColor3f(0.0f, 0.5f, 0.0f);     // Green
	     glVertex3f( 1.0f, 1.0f, -1.0f);
	     glVertex3f(-1.0f, 1.0f, -1.0f);
	     glVertex3f(-1.0f, 1.0f,  1.0f);
	     glVertex3f( 1.0f, 1.0f,  1.0f);

	     // Bottom face (y = -1.0f)
	     glColor3f(0.5f, 0.25f, 0.0f);     // Orange
	     glVertex3f( 1.0f, -1.0f,  1.0f);
	     glVertex3f(-1.0f, -1.0f,  1.0f);
	     glVertex3f(-1.0f, -1.0f, -1.0f);
	     glVertex3f( 1.0f, -1.0f, -1.0f);

	     // Front face  (z = 1.0f)
	     glColor3f(0.5f, 0.0f, 0.0f);     // Red
	     glVertex3f( 1.0f,  1.0f, 1.0f);
	     glVertex3f(-1.0f,  1.0f, 1.0f);
	     glVertex3f(-1.0f, -1.0f, 1.0f);
	     glVertex3f( 1.0f, -1.0f, 1.0f);

	     // Back face (z = -1.0f)
	     glColor3f(0.5f, 0.5f, 0.0f);     // Yellow
	     glVertex3f( 1.0f, -1.0f, -1.0f);
	     glVertex3f(-1.0f, -1.0f, -1.0f);
	     glVertex3f(-1.0f,  1.0f, -1.0f);
	     glVertex3f( 1.0f,  1.0f, -1.0f);

	     // Left face (x = -1.0f)
	     glColor3f(0.0f, 0.0f, 0.5f);     // Blue
	     glVertex3f(-1.0f,  1.0f,  1.0f);
	     glVertex3f(-1.0f,  1.0f, -1.0f);
	     glVertex3f(-1.0f, -1.0f, -1.0f);
	     glVertex3f(-1.0f, -1.0f,  1.0f);

	     // Right face (x = 1.0f)
	     glColor3f(0.5f, 0.0f, 0.5f);     // Magenta
	     glVertex3f(1.0f,  1.0f, -1.0f);
	     glVertex3f(1.0f,  1.0f,  1.0f);
	     glVertex3f(1.0f, -1.0f,  1.0f);
	     glVertex3f(1.0f, -1.0f, -1.0f);
	   glEnd();  // End of drawing color-cube



	   glTranslatef(1.5f, 7.0f, 0.0f);  // Move right and into the screen

	   glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
	      // Top face (y = 1.0f)
	      // Define vertices in counter-clockwise (CCW) order with normal
	      // pointing out
	     glColor3f(0.0f, 0.5f, 0.0f);     // Green
	     glVertex3f( 1.0f, 1.0f, -1.0f);
	     glVertex3f(-1.0f, 1.0f, -1.0f);
	     glVertex3f(-1.0f, 1.0f,  1.0f);
	     glVertex3f( 1.0f, 1.0f,  1.0f);

	     // Bottom face (y = -1.0f)
	     glColor3f(0.5f, 0.25f, 0.0f);     // Orange
	     glVertex3f( 1.0f, -1.0f,  1.0f);
	     glVertex3f(-1.0f, -1.0f,  1.0f);
	     glVertex3f(-1.0f, -1.0f, -1.0f);
	     glVertex3f( 1.0f, -1.0f, -1.0f);

	     // Front face  (z = 1.0f)
	     glColor3f(0.5f, 0.0f, 0.0f);     // Red
	     glVertex3f( 1.0f,  1.0f, 1.0f);
	     glVertex3f(-1.0f,  1.0f, 1.0f);
	     glVertex3f(-1.0f, -1.0f, 1.0f);
	     glVertex3f( 1.0f, -1.0f, 1.0f);

	     // Back face (z = -1.0f)
	     glColor3f(0.5f, 0.5f, 0.0f);     // Yellow
	     glVertex3f( 1.0f, -1.0f, -1.0f);
	     glVertex3f(-1.0f, -1.0f, -1.0f);
	     glVertex3f(-1.0f,  1.0f, -1.0f);
	     glVertex3f( 1.0f,  1.0f, -1.0f);

	     // Left face (x = -1.0f)
	     glColor3f(0.0f, 0.0f, 0.5f);     // Blue
	     glVertex3f(-1.0f,  1.0f,  1.0f);
	     glVertex3f(-1.0f,  1.0f, -1.0f);
	     glVertex3f(-1.0f, -1.0f, -1.0f);
	     glVertex3f(-1.0f, -1.0f,  1.0f);

	     // Right face (x = 1.0f)
	     glColor3f(0.5f, 0.0f, 0.5f);     // Magenta
	     glVertex3f(1.0f,  1.0f, -1.0f);
	     glVertex3f(1.0f,  1.0f,  1.0f);
	     glVertex3f(1.0f, -1.0f,  1.0f);
	     glVertex3f(1.0f, -1.0f, -1.0f);
	   glEnd();  // End of drawing color-cube

	   // Render a pyramid consists of 4 triangles
	   //   glLoadIdentity();                  // Reset the model-view matrix
	   glTranslatef(-1.5f, 0.0f, -6.0f);  // Move left and into the screen

	   glBegin(GL_TRIANGLES);           // Begin drawing the pyramid with 4 triangles
	      // Front
	      glColor3f(1.0f, 0.0f, 0.0f);     // Red
	      glVertex3f( 0.0f, 1.0f, 0.0f);
	      glColor3f(0.0f, 1.0f, 0.0f);     // Green
	      glVertex3f(-1.0f, -1.0f, 1.0f);
	      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	      glVertex3f(1.0f, -1.0f, 1.0f);

	      // Right
	      glColor3f(1.0f, 0.0f, 0.0f);     // Red
	      glVertex3f(0.0f, 1.0f, 0.0f);
	      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	      glVertex3f(1.0f, -1.0f, 1.0f);
	      glColor3f(0.0f, 1.0f, 0.0f);     // Green
	      glVertex3f(1.0f, -1.0f, -1.0f);

	      // Back
	      glColor3f(1.0f, 0.0f, 0.0f);     // Red
	      glVertex3f(0.0f, 1.0f, 0.0f);
	      glColor3f(0.0f, 1.0f, 0.0f);     // Green
	      glVertex3f(1.0f, -1.0f, -1.0f);
	      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	      glVertex3f(-1.0f, -1.0f, -1.0f);

	      // Left
	      glColor3f(1.0f,0.0f,0.0f);       // Red
	      glVertex3f( 0.0f, 1.0f, 0.0f);
	      glColor3f(0.0f,0.0f,1.0f);       // Blue
	      glVertex3f(-1.0f,-1.0f,-1.0f);
	      glColor3f(0.0f,1.0f,0.0f);       // Green
	      glVertex3f(-1.0f,-1.0f, 1.0f);
	   glEnd();   // Done drawing the pyramid
}

/* Handler for window-repaint event. Called back when the window first
   appears and whenever the window needs to be re-painted. */
void renderCB(VRDataIndex* index) {


}


// The tail end of the drawing function above has been chopped off and
// put into this swap function so it can be called in sync with all
// the other processes.
void swapCB() {

   // Swap the front and back frame buffers (double buffering)
   //glutSwapBuffers();
}


/* Handler for window re-size event. Called back when the window first
   appears and whenever the window is re-sized with its new width and
   height */
void reshape() {  // GLsizei for non-negative integer

	GLfloat width = 640;
	GLfloat height = 480;

   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;

   // Set the viewport to cover the new window
   glViewport(0, 0, (GLsizei)width, (GLsizei)height);

   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset

   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(1.6*45.0f, aspect, 0.1f, 100.0f);

   //The .m is the GLfloat* you are accessing
   //glMultMatrix( GLKMatrix4MakePerspective(45.0f, aspect, 0.1f,100.0f ).m );
}

// The mouse and special key event stuff that was here is now gone.

// This is the event handler.  Takes an event name and a pointer to
// the dataIndex where you can find that event's data.
void eventCB(const std::string &eventName, VRDataIndex *dataIndex) {

  // The event can be examined here.
  std::cout << std::endl << "examining the data..." << std::endl;
  dataIndex->printStructure(eventName);

  // The event handler's actions are here.
  if (eventName.compare("/HeadAngleEvent") == 0) {

    // If it's a head angle event, grab the two angles and calculate
    // the eye position and direction.
    horizAngle = dataIndex->getValue("/HeadAngleEvent/horizAngle");
    vertAngle = dataIndex->getValue("/HeadAngleEvent/vertAngle");

    cameraPos[0] = radius * cos(horizAngle) * cos(vertAngle);
    cameraPos[1] = -radius * sin(vertAngle);
    cameraPos[2] = radius * sin(horizAngle) * cos(vertAngle);

    cameraAim[0] = cos(horizAngle) * sin(vertAngle);
    cameraAim[1] = cos(vertAngle);
    cameraAim[2] = sin(horizAngle) * sin(vertAngle);

  } else if (eventName.compare("/RadiusEvent") == 0) {

    // This is a radius event.  Here's a condition for copying the event
    // data, just to show we can do this.
    if ((double)dataIndex->getValue("/RadiusEvent/radius") > 2.0) {
      dataIndex->addData("/MVR/VRDisplayDevices/radius",
                         (double)dataIndex->getValue("/RadiusEvent/radius"));
    }

    // Perhaps there should be a
    //    dataIndex->copyValue("/RadiusEvent/radius",
    //                         "/MVR/VRDisplayDevices/radius");

    // Notice that the radius itself is set here.  Asking for a
    // variable using a namespace allows there to be a default value
    // defined at a higher level.  So if the specific display
    // overrides the default radius value, it will not be affected by
    // the event.

    radius = dataIndex->getValue("radius", "/MVR/VRDisplayDevices/" + MVR->getName() + "/");

    std::cout << "radius: " << radius << std::endl;

    // The process name could be stored in the dataIndex instead of as
    // a private class variable in VRMain.

  }

  // Print out the entire index.
  //  std::cout << "Index Structure" << std::endl;
  //  dataIndex->printStructure();
}
