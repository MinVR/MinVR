/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <main/VRLocalAppLauncher.h>

namespace MinVR {

    
VRLocalAppLauncher::VRLocalAppLauncher(int argc, char** argv) {
    _program = std::string(argv[0]);
    for (int i = 1; i < argc ; i ++) {
        if (i > 1) {
            _origArgs += " ";
        }
        _origArgs += argv[i];
    }
}
    
VRLocalAppLauncher::~VRLocalAppLauncher() {
}

    
std::string
VRLocalAppLauncher::generateCommandLine(const std::string& processSpecificArgs) const {
    return _program + " " + _origArgs + " " + argsToData(processSpecificArgs);
}
    


} /* namespace MinVR */

