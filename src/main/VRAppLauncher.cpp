/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "VRAppLauncher.h"
#include <config/base64/base64.h>

namespace MinVR {


    
std::string VRAppLauncher::argsToData(const std::string &argStr) {
    std::string encodedData = base64_encode((unsigned char const* )argStr.c_str(), argStr.size());
    return "MINVR_DATA=" + encodedData;
}

std::string VRAppLauncher::dataToArgs(const std::string &minvrDataStr) {
    int pos = minvrDataStr.find("MINVR_DATA=");
    if (pos != std::string::npos) {
        return base64_decode(minvrDataStr.substr(11));
    }
    else {
        return base64_decode(minvrDataStr);
    }
}

    
    
} // end namespace
