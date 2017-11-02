/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRFREEGLUTINPUTDEVICE_H_
#define VRFREEGLUTINPUTDEVICE_H_


#include <map>
#include <vector>
#include <string>

#include <config/VRDataQueue.h>
#include <input/VRInputDevice.h>
#include <plugin/VRPlugin.h>


namespace MinVR {

/** A VRInputDevice that polls input events (mouse, keyboard, window resize, etc.) 
    for all of the active GLFWWindows and returns input in MinVR event format.
 */
class VRFreeGLUTInputDevice : public VRInputDevice {
public:
	PLUGIN_API VRFreeGLUTInputDevice();
	PLUGIN_API virtual ~VRFreeGLUTInputDevice();

	PLUGIN_API void appendNewInputEventsSinceLastCall(VRDataQueue* queue);

    PLUGIN_API void addWindow(int window);

	PLUGIN_API void keyCallback(int key, int x, int y, bool isDown, bool isSpecial = false);
    PLUGIN_API void cursorPositionCallback(int x, int y, bool isActive);
  	PLUGIN_API void mouseButtonCallback(int button, int state, int x, int y);
  	PLUGIN_API void mouseWheelCallback(int wheel, int direction, int x, int y);
  
private:
	std::vector<VRDataIndex> _events;
    std::vector<int > _windows;
};


} /* namespace MinVR */

#endif /* GLFWINPUTDEVICE_H_ */
