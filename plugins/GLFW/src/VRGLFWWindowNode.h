/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GLFWWINDOW_H_
#define GLFWWINDOW_H_

#include <display/VRGraphicsWindowNode.h>
#include <GLFW/glfw3.h>

namespace MinVR {

class VRGLFWWindowNode : public VRGraphicsWindowNode {
public:
	PLUGIN_API VRGLFWWindowNode(const VRRect& rect);
	PLUGIN_API virtual ~VRGLFWWindowNode();

	PLUGIN_API GLFWwindow* getWindow() const {
		return window;
	}

protected:
	std::string getContextType();
	void setCurrentContext();
	void clearCurrentContext();
	void swapBuffers();
	void flush();
	void finish();

private:
	GLFWwindow* window;
};


class VRGLFWGraphicsWindowNodeFactory : public VRDisplayNodeFactory {
public:
	PLUGIN_API GlfwWindowFactory(GlfwInputDevice* inputDevice, VRSystem* vrSystem);
	PLUGIN_API virtual ~GlfwWindowFactory();

	PLUGIN_API VRGraphicsWindowNode* createWindow(VRDataIndex& config, const std::string nameSpace, std::string type);

private:
	GlfwInputDevice* inputDevice;
};



} /* namespace MinVR */

#endif /* GLFWWINDOW_H_ */
