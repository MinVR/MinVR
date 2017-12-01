/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Ben Knorlein
 */

#ifndef VRFREEGLUTWINDOWTOOLKIT_H_
#define VRFREEGLUTWINDOWTOOLKIT_H_

#include <vector>

#include <display/VRWindowToolkit.h>
#include <main/VRFactory.h>
#include <main/VRMainInterface.h>
#include "VRFreeGLUTInputDevice.h"


namespace MinVR {

class VRMain;

/** Knows how to create windows using the GLFW library for cross-platform
    graphics window support and creates a virtual VRInputDevice to collect
    keyboard and mouse input from these windows.
 */ 
class VRFreeGLUTWindowToolkit : public VRWindowToolkit {
public:
	PLUGIN_API VRFreeGLUTWindowToolkit(VRMainInterface *vrMain);
	PLUGIN_API virtual ~VRFreeGLUTWindowToolkit();

    PLUGIN_API std::string getName() const { return "VRFreeGLUTWindowToolkit"; }
  
	PLUGIN_API int createWindow(VRWindowSettings settings);

	PLUGIN_API void makeWindowCurrent(int windowID);

    PLUGIN_API void destroyWindow(int windowID);
  
	PLUGIN_API void swapBuffers(int windowID);

	//PLUGIN_API FreeGLUTwindow* getWindow(int windowID) {
	//	return _windows[windowID];
	//}

	PLUGIN_API static VRFreeGLUTWindowToolkit* create(VRMainInterface *vrMain, VRDataIndex *config,const std::string &nameSpace);

private:
	VRMainInterface *_vrMain;
	std::vector<int> _windows;
	VRFreeGLUTInputDevice *_inputDev;
};





} /* namespace MinVR */

#endif /* GLFWWINDOW_H_ */
