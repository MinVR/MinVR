
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
 	bool visible;
 	int sharedContextGroupID;
 	int contextVersionMajor;
 	int contextVersionMinor;
 	int rgbBits;
 	int alphaBits;
 	int depthBits;
 	int stencilBits;
 	bool fullScreen;
 	bool resizable;
 	bool allowMaximize;
 	bool gpuAffinity;
 	bool debugContext;
 	int msaaSamples;

// 	Window1_Height          1300
// 	Window1_X               1681
// 	Window1_Y               0
// 	Window1_RGBBits         8
// 	Window1_AlphaBits       8
// 	Window1_DepthBits       24
// 	Window1_StencilBits     8
// 	Window1_MSAASamples     1
// 	Window1_FullScreen      0
// 	Window1_Stereo          1
// 	Window1_Resizable       0
// 	Window1_AllowMaximize   0
// 	Window1_Framed          0
// 	Window1_Caption         MinVR Cave Sides
// 	Window1_StereoType	    QuadBuffered
// 	Window1_UseDebugContext 0
// 	Window1_UseGPUAffinity  1
// 	Window1_Visible         0
};


/** Abstract base class for window toolkits (GLFW, glut, etc.) that are implemented
    in plugins.
 */
class VRWindowToolkit {
public:
	virtual ~VRWindowToolkit() {}

  virtual std::string getName() const = 0;

	virtual int createWindow(VRWindowSettings settings) = 0;

	virtual void destroyWindow(int windowID) {
		std::cerr << "destroyWindow() not enabled in this VRWindowToolkit." << std::endl;
	}

	virtual void makeWindowCurrent(int windowID) {
		std::cerr << "makeWindowCurrent() not enabled in this VRWindowToolkit." << std::endl;
	}

	virtual void swapBuffers(int windowID) {
		std::cerr << "swapBuffers() not enabled in this VRWindowToolkit." << std::endl;
	}

	virtual void getFramebufferSize(int windowID, int& width, int& height) {
		std::cerr << "getFramebufferSize() not enabled in this VRWindowToolkit." << std::endl;
	}

	static std::string getAttributeName(){ return "windowtoolkitType"; };
};


} // end namespace

#endif
