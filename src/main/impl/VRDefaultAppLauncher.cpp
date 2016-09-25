/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <main/impl/VRDefaultAppLauncher.h>
#include <config/base64/base64.h>

namespace MinVR {

VRDefaultAppLauncher::VRDefaultAppLauncher(int argc, char** argv, const std::string& customInitString) : encoded(false) {
	program = std::string(argv[0]);

	for(int i = 1; i < argc ; i ++){
		std::string arg(argv[i]);
		int pos = arg.find("MINVR_DATA=");
		if(pos != std::string::npos){
			initString = base64_decode(arg.substr(11));
			encoded = true;
		}
		else {
			if (i > 1) {
				cmd += " ";
			}

			cmd += argv[i];
		}
	}

	if (customInitString.size() > 0) {
		cmd = customInitString;
	}
}

VRDefaultAppLauncher::~VRDefaultAppLauncher() {
}

std::string VRDefaultAppLauncher::generateCommandLine(
		const std::string& initString) const  {

	std::string encodedData = base64_encode((unsigned char const* )initString.c_str(), initString.size());
	return program + " " + cmd + " MINVR_DATA=" + encodedData;
}

} /* namespace MinVR */

