#include <iostream>
#include <vector>
#include <string>
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

//-----------------Call back prototypes----------------
void initGL();
void render(VRRenderState& state);
void handleEvent(const std::string &eventName, VRDataIndex *dataIndex);

//--------------------Variables-------------------------

VRMain *MVR;
bool isRunning = true;

//------------------ Main Loop -------------------------

int main(int argc, char **argv) {

	// Initialize VRMain and register callbacks
	MVR = new VRMain();
	if (argc > 1) {
		MVR->initialize(argv[1],argv[2]);
	}
	MVR->registerEventCallback(&handleEvent);

	// Get display device (Composite of all available display devices, but looks like one device)
	VRDisplayDevice* display = MVR->getDisplay();

	// Initialize display contexts
	display->use(initGL);

	// Loop until escape key is hit or main display is closed
	while (display->isOpen() && isRunning)
	{
		// Synchronize events
		MVR->synchronizeAndProcessEvents();

		// Render the scene on all displays (passing render function into display)
		display->startRendering(render);
		MVR->renderEverywhere();
		display->finishRendering();
	}

	delete MVR;
}

//-------------------Call back functions-----------------------------------

/* Graphics variable */
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

	cameraPos[0] = radius * cos(horizAngle) * cos(vertAngle);
	cameraPos[1] = -radius * sin(vertAngle);
	cameraPos[2] = radius * sin(horizAngle) * cos(vertAngle);

	cameraAim[0] = cos(horizAngle) * sin(vertAngle);
	cameraAim[1] = cos(vertAngle);
	cameraAim[2] = sin(horizAngle) * sin(vertAngle);
}

/* Render function */
void render(VRRenderState& state) {

	if (!state.display->allowGraphics())
	{
		cout << "Command line only device: " << state.display->getName() << endl;
		return;
	}

	GLfloat width = state.display->getWidth();
	GLfloat height = state.display->getHeight();

	// Compute aspect ratio of the new window
	if (height == 0) height = 1;                // To prevent divide by 0
	GLfloat aspect = (GLfloat)width / (GLfloat)height;

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

}

// This is the event handler.  Takes an event name and a pointer to
// the dataIndex where you can find that event's data.
void handleEvent(const std::string &eventName, VRDataIndex *dataIndex) {

	// If escape is pressed, exit program
	if (eventName == "/keyboard")
	{
		std::string val = dataIndex->getValue("value", eventName);
		if (val == "ESC_down" || val == "Q_down")
		{
			isRunning = false;
		}
		else if (val == "LEFT_down" || val == "LEFT_repeat")
		{
			horizAngle -= incAngle;
		}else if (val == "RIGHT_down" || val == "RIGHT_repeat")
		{
			horizAngle += incAngle;
		}else if (val == "UP_down" || val == "UP_repeat")
		{
			vertAngle -= incAngle;
		}else if (val == "DOWN_down" || val == "DOWN_repeat")
		{
			vertAngle += incAngle;
		}

		if (horizAngle > 6.283185) horizAngle -= 6.283185;
		if (horizAngle < 0.0) horizAngle += 6.283185;

		if (vertAngle > 6.283185) vertAngle -= 6.283185;
		if (vertAngle < 0.0) vertAngle += 6.283185;

		cameraPos[0] = radius * cos(horizAngle) * cos(vertAngle);
		cameraPos[1] = -radius * sin(vertAngle);
		cameraPos[2] = radius * sin(horizAngle) * cos(vertAngle);

		cameraAim[0] = cos(horizAngle) * sin(vertAngle);
		cameraAim[1] = cos(vertAngle);
		cameraAim[2] = sin(horizAngle) * sin(vertAngle);

		cout << val << endl;
	}
}
