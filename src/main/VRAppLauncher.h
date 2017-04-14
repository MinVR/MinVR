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

/** VRAppLauncher is a helper class for generating a command line to
    spawn sub-processes by starting a new process with special command line
    arguments.  This is complicated by the need for MinVR to use command
    line arguments to pass information from one process to another and the
    simultaneous desire to not force application programmers into adopting a
    MinVR-specific command line convention.  The solution is a compromise.
    Application programmers do not have to use the full MinVR command line
    convention, but they must at least allow for one special argument of the 
    form MINVR_DATA=xxxxx to pass through whatever command line parsing they
    implement.  This special argument is what MinVR uses to pass 
    subprocess-specific config info from a parent process to child.
 
    MinVR currently uses a VRAppLauncher in two cases.  First, when a config 
    file specifies that MinVR should run more than one VRSetup, MinVR should
    fork and create one sub-process for each VRSetup.  However, since Windows 
    does not support forking, the way this is accomplished on Windows
    systems is to start a new instance of the same process, pass it all of the
    command line arguments sent to the original process, and one additional
    argument to tell it exactly which VRSetup to run.  Second, VR environments
    running on a cluster of multiple computers require MinVR to create a command
    line to ssh to each node of the cluster and start a new process there.  In
    the future, additional launchers may also be useful.  For example, perhaps 
    a plugin will require loading another application first or an application is
    interpreted (i.e. running a python script).
 */
class VRAppLauncher {
public:
	virtual ~VRAppLauncher() {}

	/** Returns the complete command line to call to start a new process and
        pass the provided process-specific configuration settings to the new 
        process.  The processSpecificConfigArgs string should be formated
        following the same conventions as the builtin MinVR command line args.
     */
	virtual std::string generateCommandLine(const std::string& processSpecificArgs) const = 0;

    /** Returns the full path to the executable (usually the first command line 
        parameter).
     */
	virtual const std::string& getExecutable() const = 0;

    
    static std::string argsToData(const std::string &args);
    static std::string dataToArgs(const std::string &minvrData);

};



} /* namespace MinVR */

#endif /* VRAPPLAUNCHER_H_ */
