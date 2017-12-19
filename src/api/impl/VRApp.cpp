/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "api/VRApp.h"
#include "api/VRAudioState.h"
#include "api/VRButtonEvent.h"
#include "api/VRConsoleState.h"
#include "api/VRCursorEvent.h"
#include "api/VRGraphicsState.h"
#include "api/VRHapticsState.h"
#include "api/VRTrackerEvent.h"

#include <main/VRMain.h>


namespace MinVR {


/** The only purpose of this class is to keep implementation details out of the
    .h files that make up the MinVR API.  Advanced users could create their own
    class that inherits directly from the VR*Handler interfaces just as this
    class does, but users of the API do not need to know how to do this, rather
    they simply subclass from VRApp and override the virtual methods provided
    there as needed.
*/

class VRAppInternal : public VREventHandler, public VRRenderHandler {

public:
	VRAppInternal(int argc, char** argv, VRApp *app) : _app(app) {
		_main = new VRMain();

        _main->addEventHandler(this);
        _main->addRenderHandler(this);
        _main->initialize(argc, argv);
	}

	virtual ~VRAppInternal() {
		delete _main;
	}

	void onVREvent(const VRDataIndex &eventData) {
        std::string type;
        if (eventData.exists("EventType")) {
            type = (VRString)eventData.getValue("EventType");
        }
        else {
            VRERROR("VRAppInternal::onVREvent() received an event named " + eventData.getName() + " of unknown type.",
                    "All events should have a data field named EventType but none was found for this event.");
        }

        if (type == "AnalogUpdate") {
            _app->onAnalogChange(VRAnalogEvent(eventData));
        }
        else if (type == "ButtonDown") {
            _app->onButtonDown(VRButtonEvent(eventData));
        }
        else if (type == "ButtonUp") {
            _app->onButtonUp(VRButtonEvent(eventData));
        }
        else if (type == "ButtonRepeat") {
            // intentionally not forwarding ButtonRepeat events since repeats are
            // not reported consistently on all systems and for VR apps we generally
            // just listen for downs and ups, it's an automatic repeat if you
            // have received a down and have not received a corresponding up.
        }
        else if (type == "CursorMove") {
            _app->onCursorMove(VRCursorEvent(eventData));
        }
        else if (type == "TrackerMove") {
            _app->onTrackerMove(VRTrackerEvent(eventData));
        }
        else {
            VRERROR("VRAppInternal::onVREvent() received an event of unknown type: " + type,
                    "Perhaps an input device that sends a new type of event was rencently added.");
        }
	}

    void onVRRenderContext(const VRDataIndex &renderData) {
        if (renderData.exists("IsGraphics")) {
            _app->onRenderGraphicsContext(VRGraphicsState(renderData));
        }
        else if (renderData.exists("IsAudio")) {
            _app->onRenderAudio(VRAudioState(renderData));
        }
        else if (renderData.exists("IsConsole")) {
            _app->onRenderConsole(VRConsoleState(renderData));
        }
        else if (renderData.exists("IsHaptics")) {
            _app->onRenderHaptics(VRHapticsState(renderData));
        }
        else {
            VRERROR("VRAppInternal::onRenderContext() received an unknown type of render callback",
                    "Perhaps a new type of display node was recently added.");
        }
    }


    void onVRRenderScene(const VRDataIndex &renderData) {
        if (renderData.exists("IsGraphics")) {
            _app->onRenderGraphicsScene(VRGraphicsState(renderData));
        }
        else if (renderData.exists("IsAudio")) {
            // nothing to do, already called onRenderAudio() during onVRRenderContext
        }
        else if (renderData.exists("IsConsole")) {
            // nothing to do, already called onRenderConsole() during onVRRenderContext
        }
        else if (renderData.exists("IsHaptics")) {
            // nothing to do, already called onRenderHaptics() during onVRRenderContext
        }
        else {
            VRERROR("VRAppInternal::onRenderScene() received an unknown type of render callback",
                    "Perhaps a new type of display node was recently added.");
        }
    }

    void run() {
        while (_main->mainloop()) {}
	}

	void shutdown() {
 		_main->shutdown();

	}

    std::string getVRSetupName() {
        return _main->getName();
    }

    int getLeftoverArgc() {
        return _main->getLeftoverArgc();
    }

    char** getLeftoverArgv() {
        return _main->getLeftoverArgv();
    }

private:

    VRApp *_app;
	VRMain *_main;
};


VRApp::VRApp(int argc, char** argv) {
	_internal = new VRAppInternal(argc, argv, this);
}

VRApp::~VRApp()  {
	_internal->shutdown();
	delete _internal;
}

void VRApp::run()  {
	_internal->run();
}

void VRApp::shutdown() {
	_internal->shutdown();
}

std::string VRApp::getVRSetupName() {
    return _internal->getVRSetupName();
}

int VRApp::getLeftoverArgc() {
  return _internal->getLeftoverArgc();
}

char** VRApp::getLeftoverArgv() {
  return _internal->getLeftoverArgv();
}

}
