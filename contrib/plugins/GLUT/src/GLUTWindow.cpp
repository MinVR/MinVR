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

void GLUTWindow::initialize() {
	int argc = 0;
	char* argv[0];
	glutInit(&argc, argv);            // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH); // Enable double buffered mode
	glutInitWindowSize(width, height);   // Set the window's initial width & height
	glutInitWindowPosition(x, y); // Position the window's initial top-left corner
	glutCreateWindow(getName().c_str());          // Create window with the given title
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

VRDisplayDevice* GLUTWindowFactory::createDisplay(const std::string type,
		const std::string name, VRDataIndex& config,
		VRDisplayDeviceFactory* factory) {

	if (type == "glut_display")
	{
		int xOffset = config.getValue("xOffset", name);
		int yOffset = config.getValue("yOffset", name);
		int width = config.getValue("width", name);
		int height = config.getValue("height", name);

		GLUTWindow* window = new GLUTWindow(xOffset, yOffset, width, height);

		return window;
	}

	return NULL;
}

GLUTFrameController::GLUTFrameController() {
}

GLUTFrameController::~GLUTFrameController() {
}

VRDisplayFrameAction* glutFrameAction = NULL;

void MinVR_glutWindowLoop()
{
	//std::cout << "in loop" << std::endl;
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
	glutDisplayFunc(MinVR_glutWindowLoop);

	//std::cout << "start loop" << std::endl;

	glutMainLoop();

	return false;
}


} /* namespace MinVR */

