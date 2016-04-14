/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "VRGLFWWindowToolkit.h"
#include <main/VRMainInterface.h>
#include <iostream>

namespace MinVR {

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}


VRGLFWWindowToolkit::VRGLFWWindowToolkit(VRMainInterface *vrMain) : _vrMain(vrMain), _inputDev(NULL) {
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
	   	std::cout << "GLFW Init failed." << std::endl;
	    exit(0);
	}

	_inputDev = new VRGLFWInputDevice();
}

VRGLFWWindowToolkit::~VRGLFWWindowToolkit() {
	if (_inputDev != NULL) {
		delete _inputDev;
	}
    glfwTerminate();
}


int
VRGLFWWindowToolkit::createWindow(VRWindowSettings settings) {
	glfwDefaultWindowHints();

	if (settings.quadBuffered) {
		glfwWindowHint(GLFW_STEREO, true);
	}

	GLFWwindow* window = glfwCreateWindow(settings.width, settings.height, settings.caption, NULL, NULL);
	if (!window) {
		std::cout << "Error creating window." << std::endl;
	}

	std::cout << "Created window." << std::endl;
	glfwSetWindowPos(window, settings.xpos, settings.ypos);
	glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
	glfwMakeContextCurrent(NULL);

	_inputDev->addWindow(window);
	if (_windows.size() == 0) {
		// if this is the first window created, then register the virtual input device
		// with VRMain so that VRMain will start polling GLFW for input events
		_vrMain->addInputDevice(_inputDev);
	}

	_windows.push_back(window);
	return _windows.size()-1;
}

void 
VRGLFWWindowToolkit::destroyWindow(int windowID) {
	// TODO: _inputDev->removeWindow(_windows[windowID]);
	glfwDestroyWindow(_windows[windowID]);
}

void 
VRGLFWWindowToolkit::makeWindowCurrent(int windowID) {
	if ((windowID >=0) && (windowID < _windows.size())) {
		glfwMakeContextCurrent(_windows[windowID]);
	}
	else {
		glfwMakeContextCurrent(NULL);
	}
}

void 
VRGLFWWindowToolkit::swapBuffers(int windowID) {
	glfwMakeContextCurrent(_windows[windowID]);
	glfwSwapBuffers(_windows[windowID]);
}


VRWindowToolkit*
VRGLFWWindowToolkitFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) {
	std::string nameSpace = nameSpace + "/" + valName;

	std::string type = config->getValue("Type", nodeNameSpace);
	if (type != "VRGLFWWindowToolkit") {
		// This factory cannot create the type specified
		return NULL;
	}

	VRWindowToolkit *winToolkit = new VRGLFWWindowToolkit(vrMain);
	return winToolkit;
}


} /* namespace MinVR */

