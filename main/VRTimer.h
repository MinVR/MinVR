/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRTIMER_H_
#define VRTIMER_H_

#include <string>

namespace MinVR {

class VRTimer {
public:
	virtual ~VRTimer() {}

	virtual std::string getName() = 0;
	virtual double getTime() = 0;
};

} /* namespace MinVR */

#endif /* VRTIMER_H_ */
