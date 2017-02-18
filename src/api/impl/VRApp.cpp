/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "../VRApp.h"

//#include <main/VRAudioHandler.h>
#include <main/VREventHandler.h>
#include <main/VRGraphicsHandler.h>
//#include <main/VRHapticsHandler.h>
#include <main/VRMain.h>


namespace MinVR {


/** The only purpose of this class is to keep implementation details out of the .h files
    that make up the MinVR API.  Advanced users could create their own class that inherits
    directly from the VR*Handler interfaces just as this class does, but users of the API
    do not need to know how to do this, rather they simply subclass from VRApp and override
    the virtual methods provided there as needed.
*/
  class VRAppInternal : //public VRAudioHandler,
					  public VREventHandler, 
                      public VRGraphicsHandler { //,
                      //public VRHapticsHandler {
public:
	VRAppInternal(int argc, char** argv, const std::string& configFile, VRApp *app) : _app(app), _running(false) {
		_main = new VRMain();
		_main->initialize(argc, argv, configFile);
		_main->addEventHandler(this);
		_main->addRenderHandler(this);
	}

	virtual ~VRAppInternal() {
		delete _main;
	}

	void onVREvent(const VREvent &event) {
		_app->onVREvent(event);
	}

//	void onVRRenderAudio(const VRAudioState& state) {
//		_app->onVRRenderAudio(state);
//	}

	void onVRRenderGraphics(const VRGraphicsState& state) {
		_app->onVRRenderGraphics(state);
	}

	void onVRRenderGraphicsContext(const VRGraphicsState& state) {
		_app->onVRRenderGraphicsContext(state);
	}

//	void onVRRenderHaptics(const VRHapticsState& state) {
//		_app->onVRRenderHaptics(state);
//	}

    bool isRunning() {
        return _running;
    }
                        
	void run() {
        _running = true;
        while (_main->mainloop()) {}
	}

	void shutdown() {
        _running = false;
		_main->shutdown();
	}

private:

    VRApp *_app;
	VRMain *_main;
    bool _running;
};



VRApp::VRApp(int argc, char** argv, const std::string& configFile) {
	_internal = new VRAppInternal(argc, argv, configFile, this);
}

VRApp::~VRApp()  {
	_internal->shutdown();
	delete _internal;
}

void VRApp::run()  {
	_internal->run();
}

bool VRApp::isRunning() const  {
  return _internal->isRunning();
}
  
void VRApp::shutdown() {
	_internal->shutdown();
}


}
