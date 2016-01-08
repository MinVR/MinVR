/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GLFWTIMER_H_
#define GLFWTIMER_H_

#include "plugin/PluginFramework.h"
#include "main/VRTimer.h"

namespace MinVR {

class GlfwTimer : public VRTimer {
public:
	PLUGIN_API GlfwTimer();
	PLUGIN_API virtual ~GlfwTimer();

	PLUGIN_API std::string getName();
	PLUGIN_API double getTime();
};

} /* namespace MinVR */

#endif /* GLFWTIMER_H_ */
