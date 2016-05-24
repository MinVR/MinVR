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

	/*Window1_Width           2550
Window1_Height          1300
Window1_X               1681
Window1_Y               0
Window1_RGBBits         8
Window1_AlphaBits       8
Window1_DepthBits       24
Window1_StencilBits     8
Window1_MSAASamples     1
Window1_FullScreen      0
Window1_Stereo          1
Window1_Resizable       0
Window1_AllowMaximize   0
Window1_Framed          0
Window1_Caption         MinVR Cave Sides
Window1_StereoType	    QuadBuffered
Window1_UseDebugContext 0
Window1_UseGPUAffinity  1
Window1_Visible         0*/

	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	//glfwWindowHint(GLFW_DECORATED, settings->framed);
	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_RESIZABLE, 0);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	//glfwWindowHint(GLFW_VISIBLE, 0);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 0);

	/*glfwWindowHint(GLFW_ALPHA_BITS, settings.->alphaBits);
	glfwWindowHint(GLFW_DEPTH_BITS, settings->depthBits);
	glfwWindowHint(GLFW_DECORATED, settings->framed);
	glfwWindowHint(GLFW_SAMPLES, settings->msaaSamples);
	glfwWindowHint(GLFW_RESIZABLE, settings->resizable);
	glfwWindowHint(GLFW_RED_BITS, settings->rgbBits);
	glfwWindowHint(GLFW_GREEN_BITS, settings->rgbBits);
	glfwWindowHint(GLFW_BLUE_BITS, settings->rgbBits);
	glfwWindowHint(GLFW_STENCIL_BITS, settings->stencilBits);
	glfwWindowHint(GLFW_STEREO, settings->stereo && settings->stereoType == WindowSettings::STEREOTYPE_QUADBUFFERED);
	glfwWindowHint(GLFW_VISIBLE, settings->visible);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, settings->useDebugContext);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);*/

    if (settings.quadBuffered) {
  		glfwWindowHint(GLFW_STEREO, true);
    }

	GLFWwindow* window = glfwCreateWindow(settings.width, settings.height, settings.caption.c_str(), NULL, NULL);
	if (!window) {
		std::cout << "Error creating window." << std::endl;
	}

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
	VRWindowToolkit *winToolkit = new VRGLFWWindowToolkit(vrMain);
	return winToolkit;
}


} /* namespace MinVR */

