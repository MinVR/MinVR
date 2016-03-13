/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <GlfwWindow.h>
#include <iostream>

namespace MinVR {

GlfwWindow::GlfwWindow(GlfwInputDevice* inputDevice) {
	glfwDefaultWindowHints();

	/*if (isQuadbuffered())
	{
		glfwWindowHint(GLFW_STEREO, true);
	}*/

	window = glfwCreateWindow(500, 500, "MinVR Window", NULL, NULL);
	if (!window)
	{
		std::cout << "Error creating window." << std::endl;
	}

	std::cout << "Created window." << std::endl;
	glfwSetWindowPos(window, 0, 0);
	glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    inputDevice->registerGlfwWindow(window);

	glfwMakeContextCurrent(NULL);
}

GlfwWindow::~GlfwWindow() {
	glfwDestroyWindow(window);
}

std::string GlfwWindow::getContextType() {
	return "opengl";
}

void GlfwWindow::setCurrentContext() {
	glfwMakeContextCurrent(window);
}

void GlfwWindow::clearCurrentContext() {
	glfwMakeContextCurrent(NULL);
}

void GlfwWindow::swapBuffers() {
	glfwSwapBuffers(window);
}

void GlfwWindow::flush() {
	glFlush();
}

void GlfwWindow::finish() {
	glFinish();
}


} /* namespace MinVR */

