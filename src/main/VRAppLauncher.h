/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRAPPLAUNCHER_H_
#define VRAPPLAUNCHER_H_

#include <string>

namespace MinVR {

class VRAppLauncher {
public:
	virtual ~VRAppLauncher() {}

	virtual std::string generateCommandLine(const std::string& initString) const = 0;
	virtual const std::string& getInitString() const = 0;
};



} /* namespace MinVR */

#endif /* VRAPPLAUNCHER_H_ */
