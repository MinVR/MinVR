/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GLFWTIMER_H_
#define GLFWTIMER_H_

#include "main/VRTimer.h"

namespace MinVR {

class GlfwTimer : public VRTimer {
public:
	GlfwTimer();
	virtual ~GlfwTimer();

	std::string getName();
	double getTime();
};

} /* namespace MinVR */

#endif /* GLFWTIMER_H_ */
