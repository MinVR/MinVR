/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GLFWWINDOWTOOLKIT_H_
#define GLFWWINDOWTOOLKIT_H_

#include <vector>

#include <GLFW/glfw3.h>
#include <display/VRWindowToolkit.h>
#include <main/VRFactory.h>
#include <main/VRMainInterface.h>
#include "VRGLFWInputDevice.h"
#include <map>


namespace MinVR {

class VRMain;

/** Knows how to create windows using the GLFW library for cross-platform
    graphics window support and creates a virtual VRInputDevice to collect
    keyboard and mouse input from these windows.
 */ 
class VRGLFWWindowToolkit : public VRWindowToolkit {
public:
	PLUGIN_API VRGLFWWindowToolkit(VRMainInterface *vrMain);
	PLUGIN_API virtual ~VRGLFWWindowToolkit();

    PLUGIN_API std::string getName() { return "VRGLFWWindowToolkit"; }
  
	PLUGIN_API int createWindow(VRWindowSettings settings);

	PLUGIN_API void makeWindowCurrent(int windowID);

    PLUGIN_API void destroyWindow(int windowID);
  
	PLUGIN_API void swapBuffers(int windowID);

	PLUGIN_API void getFramebufferSize(int windowID, int& width, int& height);

	PLUGIN_API GLFWwindow* getWindow(int windowID) {
		return _windows[windowID];
	}

	PLUGIN_API static VRWindowToolkit* create(VRMainInterface *vrMain, VRDataIndex *config,const std::string &nameSpace);

private:
	void initGLEW();

	VRMainInterface *_vrMain;
	std::vector<GLFWwindow*> _windows;
	VRGLFWInputDevice *_inputDev;
	std::map<int, GLFWwindow*> _sharedContextGroups;
};





} /* namespace MinVR */

#endif /* GLFWWINDOW_H_ */
