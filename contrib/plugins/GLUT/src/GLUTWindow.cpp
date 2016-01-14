/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <contrib/plugins/GLUT/src/GLUTWindow.h>
#include <GL/glut.h>

namespace MinVR {

GLUTWindow::GLUTWindow(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {
}

GLUTWindow::~GLUTWindow() {
}

void GLUTWindow::finishRendering() {
	glutSwapBuffers();
}

void GLUTWindow::useDisplay(const MinVR::VRDisplayAction& action) {
	useAllDisplays(action);
}

int GLUTWindow::getWidth() {
	return width;
}

int GLUTWindow::getHeight() {
	return height;
}

std::vector<std::string> events;

void GLUTWindow::appendNewInputEventsSinceLastCall(VRDataQueue& queue) {
    for (int f = 0; f < events.size(); f++)
    {
    	queue.push(events[f]);
    }

    events.clear();
}

void GLUTWindow::keyboardInput(std::string key, int x, int y,
		std::string action) {

	static VRDataIndex dataIndex;

	std::string event = key + "_" + action;
	dataIndex.addData("/keyboard/value", event);
	dataIndex.addData("/keyboard/timestamp", (int)clock());
	events.push_back(dataIndex.serialize("/keyboard"));
}

void GLUTWindow::keyboardInput(unsigned char c, int x, int y) {
	if (c == 27)
	{
		keyboardInput("ESC", x, y, "down");
		return;
	}

	char C = std::toupper((int)c);
	keyboardInput(std::string() + C, x, y, "down");
}

void GLUTWindow::keyboardUpInput(unsigned char c, int x, int y) {
	if (c == 27)
	{
		keyboardInput("ESC", x, y, "down");
		return;
	}

	char C = std::toupper((int)c);
	keyboardInput(std::string() + C, x, y, "up");
}

void GLUTWindow::keyboardSpecialInput(int key, int x, int y) {
	keyboardInput(determineSpecialKey(key), x, y, "down");
}

void GLUTWindow::keyboardSpecialUpInput(int key, int x, int y) {
	keyboardInput(determineSpecialKey(key), x, y, "up");
}

std::string GLUTWindow::determineSpecialKey(int key) {

	switch (key) {
	case GLUT_KEY_F1 :							return "F1";
	case GLUT_KEY_F2 :							return "F2";
	case GLUT_KEY_F3 :							return "F3";
	case GLUT_KEY_F4 :							return "F4";
	case GLUT_KEY_F5 :							return "F5";
	case GLUT_KEY_F6 :							return "F6";
	case GLUT_KEY_F7 :							return "F7";
	case GLUT_KEY_F8 :							return "F8";
	case GLUT_KEY_F9 :							return "F9";
	case GLUT_KEY_F10 :							return "F10";
	case GLUT_KEY_F11 :							return "F11";
	case GLUT_KEY_F12 :							return "F12";
	case GLUT_KEY_LEFT :						return "LEFT";
	case GLUT_KEY_UP :							return "UP";
	case GLUT_KEY_RIGHT :						return "RIGHT";
	case GLUT_KEY_DOWN :						return "DOWN";
	case GLUT_KEY_PAGE_UP :						return "PAGE_UP";
	case GLUT_KEY_PAGE_DOWN :					return "PAGE_DOWN";
	case GLUT_KEY_HOME :						return "HOME";
	case GLUT_KEY_END :							return "END";
	case GLUT_KEY_INSERT :						return "INSERT";
	};

	return "";
}

void GLUTWindow::initialize() {
	int argc = 0;
	char* argv[0];
	glutInit(&argc, argv);            // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH); // Enable double buffered mode
	glutInitWindowSize(width, height);   // Set the window's initial width & height
	glutInitWindowPosition(x, y); // Position the window's initial top-left corner
	glutCreateWindow(getName().c_str());          // Create window with the given title
	glutKeyboardFunc(keyboardInput);
	glutKeyboardUpFunc(keyboardUpInput);
	glutSpecialFunc(keyboardSpecialInput);
	glutSpecialUpFunc(keyboardSpecialUpInput);
}

MinVR::VRFrameController* GLUTWindow::getFrameController() {
	static GLUTFrameController frameController;
	return &frameController;
}

void GLUTWindow::startRendering(const MinVR::VRRenderer& renderer,
		VRRenderState& state) {
	startRenderingAllDisplays(renderer, state);
}


GLUTWindowFactory::GLUTWindowFactory() {
}

GLUTWindowFactory::~GLUTWindowFactory() {
}

VRInputDevice* device = NULL;

VRDisplayDevice* GLUTWindowFactory::createDisplay(const std::string type,
		const std::string name, VRDataIndex& config,
		VRDisplayDeviceFactory* factory) {

	if (type == "glut_display")
	{
		// Only one GLUT window per process
		static int numWindows = 0;
		numWindows++;
		if (numWindows > 1)
		{
			std::cout << "Only one glut window is allowed per process." << std::endl;
			return NULL;
		}

		int xOffset = config.getValue("xOffset", name);
		int yOffset = config.getValue("yOffset", name);
		int width = config.getValue("width", name);
		int height = config.getValue("height", name);


		GLUTWindow* window = new GLUTWindow(xOffset, yOffset, width, height);
		device = window;

		return window;
	}

	return NULL;
}

GLUTFrameController::GLUTFrameController() {
}

GLUTFrameController::~GLUTFrameController() {
}

VRDisplayFrameAction* glutFrameAction = NULL;

void GLUTFrameController::windowLoop()
{
	if (glutFrameAction->exec())
	{
		glutPostRedisplay();
	}
	else
	{
		exit(0);
	}
}

bool GLUTFrameController::renderFrame(VRDisplayFrameAction& frameAction) {
	glutFrameAction = &frameAction;
	glutDisplayFunc(GLUTFrameController::windowLoop);

	glutMainLoop();

	return false;
}

std::vector<VRInputDevice*> GLUTWindowFactory::create(
		VRDataIndex& dataIndex) {

	std::vector<VRInputDevice*> devices;
	if (device != NULL)
	{
		devices.push_back(device);
	}
	return devices;
}

} /* namespace MinVR */

