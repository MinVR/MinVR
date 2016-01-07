/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <GlfwTimer.h>
#include <GLFW/glfw3.h>

namespace MinVR {

GlfwTimer::GlfwTimer() {
}

GlfwTimer::~GlfwTimer() {
}

std::string GlfwTimer::getName() {
	return "GlfwTimer";
}

double GlfwTimer::getTime() {
	return glfwGetTime();
}

} /* namespace MinVR */
