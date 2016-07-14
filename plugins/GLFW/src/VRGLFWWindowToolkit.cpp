/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "GL/glew.h"
#ifdef _WIN32
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif

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

	initGLEW();

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

	glfwWindowHint(GLFW_ALPHA_BITS, settings.alphaBits);
	glfwWindowHint(GLFW_DEPTH_BITS, settings.depthBits);
	glfwWindowHint(GLFW_SAMPLES, settings.msaaSamples);
	glfwWindowHint(GLFW_RESIZABLE, settings.resizable);
	glfwWindowHint(GLFW_RED_BITS, settings.rgbBits);
	glfwWindowHint(GLFW_GREEN_BITS, settings.rgbBits);
	glfwWindowHint(GLFW_BLUE_BITS, settings.rgbBits);
	glfwWindowHint(GLFW_STENCIL_BITS, settings.stencilBits);
	glfwWindowHint(GLFW_VISIBLE, settings.visible);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, settings.debugContext);
	glfwWindowHint(GLFW_DECORATED, settings.border);
	if (settings.contextVersionMajor >= 4 || (settings.contextVersionMajor >= 3 && settings.contextVersionMinor >= 2))
	{
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings.contextVersionMajor);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings.contextVersionMinor);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	}

    if (settings.quadBuffered) {
  		glfwWindowHint(GLFW_STEREO, true);
    }

    GLFWwindow* sharedContext = NULL;
    bool foundSharedContextGroup = false;
    if (settings.sharedContextGroupID >= 0) {
    	std::map<int, GLFWwindow*>::iterator it = _sharedContextGroups.find(settings.sharedContextGroupID);
    	if (it != _sharedContextGroups.end()) {
    		foundSharedContextGroup = true;
    		sharedContext = it->second;
    	}
    }

	GLFWwindow* window = glfwCreateWindow(settings.width, settings.height, settings.caption.c_str(), NULL, sharedContext);
	if (!window) {
		std::cout << "Error creating window." << std::endl;
	}

	if (settings.sharedContextGroupID >= 0 || !foundSharedContextGroup) {
		_sharedContextGroups[settings.sharedContextGroupID] = window;
	}

#ifdef _MSC_VER2

#define MAX_AFFINITY_GPUS 16

	if (settings.gpuAffinity) {
		int currentGPU = -1;
		GPU_DEVICE gpus[MAX_AFFINITY_GPUS];
		if (WGLEW_NV_gpu_affinity) {
			int numGPUs = 0;
			for( UINT gpu = 0; true; ++gpu ) {
				HGPUNV hGPU = 0;
				if( !wglEnumGpusNV( gpu, &hGPU )) {
					break;
				}

				Rect2D win(settings->width, settings->height, settings->xPos, settings->yPos);
				int deviceIndex = 0;
				int area = win.getArea();
				int gpuArea = 0;
				gpus[gpu].cb = sizeof(GPU_DEVICE);
				while (wglEnumGpuDevicesNV(hGPU, deviceIndex, &gpus[gpu])) {


					deviceIndex++;
					int j = gpu;
					Rect2D dev(gpus[j].rcVirtualScreen.right - gpus[j].rcVirtualScreen.left, gpus[j].rcVirtualScreen.bottom - gpus[j].rcVirtualScreen.top, gpus[j].rcVirtualScreen.left, gpus[j].rcVirtualScreen.top);
					Rect2D intersection = dev.intersect(win);
					gpuArea += intersection.getArea();
					if (area <= gpuArea)
					{
						currentGPU = gpu;
						//break;
					}


				}

				if (currentGPU > 0)
				{
					break;
				}
				numGPUs++;
			}

			glfwWindowHint(GLFW_AFFINITY_GPU, currentGPU);
		}
	}
#endif

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

	glfwShowWindow(window);
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
VRGLFWWindowToolkit::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
	return new VRGLFWWindowToolkit(vrMain);
}

void VRGLFWWindowToolkit::initGLEW()
{
	// Initialize glew
	// Requires that a context exists and is current before it will work, so we create a temporary one here
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Create a debug context so glew is initialized with debug context extensions as well in case we need them later
	GLFWwindow* tempWin = glfwCreateWindow(200, 200, "Temporary", NULL, NULL);
	glfwMakeContextCurrent(tempWin);
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Error initializing GLEW." << std::endl;
	}

	glfwMakeContextCurrent(NULL);
	glfwDestroyWindow(tempWin);
}


} /* namespace MinVR */

