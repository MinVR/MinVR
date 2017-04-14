/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRLOCALAPPLANCHER_H_
#define VRLOCALAPPLANCHER_H_

#include <main/VRAppLauncher.h>

namespace MinVR {

/** Builds a command line for launching sub-processes on the local machine.
 */
class VRLocalAppLauncher : public VRAppLauncher {
public:

    /** Initialize with argc and argv because these will get passed through
        without modification to all subprocesses that are spawned.  The exe
        path and name for the current process is assumed to be in argv[0].
     */
    VRLocalAppLauncher(int argc, char** argv);
	virtual ~VRLocalAppLauncher();

	std::string generateCommandLine(const std::string& processSpecificArgs) const;

	const std::string& getExecutable() const {
		return _program;
	}

private:
	std::string _program;
	std::string _origArgs;
};

} /* namespace MinVR */

#endif /* VRDEFAULTAPPLANCHER_H_ */
