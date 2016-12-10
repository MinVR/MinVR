/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "VRGraphicsApp.h"

namespace MinVR {

VRGraphicsApp::VRGraphicsApp(int argc, char** argv, const std::string& configFile) : vrMain(NULL), running(true), frame(0) {
	vrMain = new VRMain();

	vrMain->initialize(argc, argv, configFile);

	vrMain->addEventHandler(this);
	vrMain->addRenderHandler(this);
}

VRGraphicsApp::~VRGraphicsApp()  {
	vrMain->shutdown();
	delete vrMain;
}

void VRGraphicsApp::run()  {
	while (running) {
		vrMain->mainloop();
		frame++;
	}
}

void VRGraphicsApp::shutdown() {
	running = false;
}

bool VRGraphicsApp::isRunning() const {
	return running;
}

int VRGraphicsApp::getFrame() const {
	return frame;
}

}
