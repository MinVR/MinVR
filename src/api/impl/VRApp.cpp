/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <api/VRApp.h>
#include <main/VRMain.h>
#include <iostream>

namespace MinVR {

class VRApp::VRAppInternal {
public:
	VRAppInternal() : vrMain(NULL), running(true), frame(0) {}
	VRMain *vrMain;
	bool running;
	int frame;
};

VRApp::VRApp(int argc, char** argv, const std::string& configFile) : _(*(new VRAppInternal())) {
	_.vrMain = new VRMain();

	_.vrMain->initialize(argc, argv, configFile);

	_.vrMain->addEventHandler(this);
}

VRApp::~VRApp()  {
	_.vrMain->shutdown();
	delete _.vrMain;
	delete &_;
}

void VRApp::run()  {
	while (_.running) {
		_.vrMain->mainloop();
		_.frame++;
	}
}

void VRApp::shutdown() {
	_.running = false;
}

bool VRApp::isRunning() const {
	return _.running;
}

int VRApp::getFrame() const {
	return _.frame;
}

void VRApp::addEventHandler(VREventHandler& handler) {
	_.vrMain->addEventHandler(&handler);
}

void VRApp::addRenderHandler(VRRenderHandler& handler) {
	_.vrMain->addRenderHandler(&handler);
}

} /* namespace MinVR */
