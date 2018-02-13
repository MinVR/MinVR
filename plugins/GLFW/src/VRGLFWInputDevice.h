/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GLFWINPUTDEVICE_H_
#define GLFWINPUTDEVICE_H_


#include <map>
#include <vector>
#include <string>

// This avoids APIENTRY macro redefinition warnings when including glfw3.h
#if defined (WIN32)
#define NOMINMAX
#include <winsock2.h>
#include <windows.h>
#endif

#include <GLFW/glfw3.h>

#include <config/VRDataQueue.h>
#include <input/VRInputDevice.h>
#include <plugin/VRPlugin.h>


namespace MinVR {

/** A VRInputDevice that polls input events (mouse, keyboard, window resize, etc.)
    for all of the active GLFWWindows and returns input in MinVR event format.
 */
class VRGLFWInputDevice : public VRInputDevice {
public:
	PLUGIN_API VRGLFWInputDevice();
	PLUGIN_API virtual ~VRGLFWInputDevice();

    PLUGIN_API void appendNewInputEventsSinceLastCall(VRDataQueue* queue);

    PLUGIN_API void addWindow(::GLFWwindow* window);
	// TODO: removeWindow()?
	// TODO: switch to using the windowID's used by VRGLFWWindowToolkit rather than
	// GLFWwindow pointers?

	PLUGIN_API void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	PLUGIN_API void sizeCallback(GLFWwindow* window, int width, int height);
    PLUGIN_API void cursorPositionCallback(GLFWwindow* window, float xpos, float ypos);
  	PLUGIN_API void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

private:
	std::vector<VRDataIndex> _events;
    std::vector< ::GLFWwindow*> _windows;
};


} /* namespace MinVR */

#endif /* GLFWINPUTDEVICE_H_ */
