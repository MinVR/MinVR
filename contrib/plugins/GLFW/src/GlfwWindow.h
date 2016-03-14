/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GLFWWINDOW_H_
#define GLFWWINDOW_H_

#include "plugin/PluginFramework.h"
#include "display/graphics/VRGraphicsWindowNode.h"
#include <GLFW/glfw3.h>

namespace MinVR {

class GlfwWindow : public VRGraphicsWindowNode {
public:
	PLUGIN_API GlfwWindow(const VRViewport& viewport);
	PLUGIN_API virtual ~GlfwWindow();

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

} /* namespace MinVR */

#endif /* GLFWWINDOW_H_ */
