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

/** VRAppLauncher allows applications to customize how they would like to fork a MinVR application.
	This is important for both ssh and windows forking as well as specialized ways for launching
	an application.  For example, perhaps a plugin requires loading another application first or
	an application is interpreted (i.e. running a python script).*/
class VRAppLauncher {
public:
	virtual ~VRAppLauncher() {}

	/// Returns the command line to call when forking an application.  The initString, which contains
	/// MinVR specific items will be provided.  This string needs to be passed through the command line somehow.
	/// For example, the initString may be it's own parameter (i.e. MINVR_STR=<initString>)
	virtual std::string generateCommandLine(const std::string& initString) const = 0;
	/// An App Launcher both knows how to interpret a command line and how to create one.  getInitString() will return
	/// the initString that comes in through the command line.  (i.e. it knows how to parse MINVR_STR=<initString>)
	virtual const std::string& getInitString() const = 0;
	/// Returns the full path to the executable (usually the first command line parameter)
	virtual const std::string& getExecutable() const = 0;
};



} /* namespace MinVR */

#endif /* VRAPPLAUNCHER_H_ */
