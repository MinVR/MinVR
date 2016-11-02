/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRDEFAULTAPPLANCHER_H_
#define VRDEFAULTAPPLANCHER_H_

#include <main/VRAppLauncher.h>

namespace MinVR {

class VRDefaultAppLauncher : public VRAppLauncher {
public:
	VRDefaultAppLauncher(int argc, char** argv, const std::string& customInitString = "");
	virtual ~VRDefaultAppLauncher();

	std::string generateCommandLine(const std::string& initString) const;

	bool isEncoded() const {
		return encoded;
	}

	const std::string& getInitString() const {
		return initString;
	}

	const std::string& getExecutable() const {
		return program;
	}

private:
	std::string program;
	std::string cmd;
	std::string initString;
	bool encoded;
};

} /* namespace MinVR */

#endif /* VRDEFAULTAPPLANCHER_H_ */
