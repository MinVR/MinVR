
#ifndef VRWINDOWTOOLKIT_H
#define VRWINDOWTOOLKIT_H

#include <string>
#include <iostream>


namespace MinVR {

/** New window settings are likely to be introduced over time, and there will be
    a long list of settings, so these are stored in a settings data structure to
    make these changes easier.  If you add a setting here, make sure that you also
    update the config file parsing in VRGraphicsWindowNode.cpp to handle the new
    setting.
*/
class VRWindowSettings {
public:
 	// TODO:  Add a constructor to initialize with default values.

	int xpos;
	int ypos;
	int width;
	int height;
	bool border;
	std::string caption;
 	bool quadBuffered;
 	std::string gpuAffinity;
 	// ...
};


/** Abstract base class for window toolkits (GLFW, glut, etc.) that are implemented
    in plugins.
 */
class VRWindowToolkit {
public:
	~VRWindowToolkit() {}

    virtual std::string getName() = 0;

	virtual int createWindow(VRWindowSettings settings) = 0;

	virtual void destroyWindow(int windowID) {
		std::cerr << "destroyWindow() not enabled in this VRWindowToolkit." << std::cerr;		
	}

	virtual void makeWindowCurrent(int windowID) {
		std::cerr << "makeWindowCurrent() not enabled in this VRWindowToolkit." << std::cerr;
	}

	virtual void swapBuffers(int windowID) {
		std::cerr << "swapBuffers() not enabled in this VRWindowToolkit." << std::cerr;
	}
};


} // end namespace

#endif