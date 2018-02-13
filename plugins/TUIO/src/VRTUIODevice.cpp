/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */


#include "VRTUIODevice.h"

#include <api/VRButtonEvent.h>
#include <api/VRCursorEvent.h>


#include <list>
#include <sstream>

using namespace TUIO;

namespace MinVR {


// using this rather than std::to_string() to avoid dependency on C++-11
std::string intToStr(int i) {
  std::ostringstream s;
  s << i;
  return s.str();
}




VRTUIODevice::VRTUIODevice(int port, double xScale, double yScale)
{
	_xScale = xScale;
	_yScale = yScale;
	_tuioClient = new TuioClient(port);
	_tuioClient->connect();

	if (!_tuioClient->isConnected())
	{  
		std::cout << "InputDeviceTUIOClient: Cannot connect on port " << port << "." << std::endl;
	}
}

VRTUIODevice::~VRTUIODevice()
{
	if (_tuioClient) {
		_tuioClient->disconnect();
		delete _tuioClient;
	}
}

void VRTUIODevice::appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents)
{
    std::vector<VRDataIndex> events;

	// Send out events for TUIO "cursors" by polling the TuioClient for the current state
	std::list<TuioCursor*> cursorList = _tuioClient->getTuioCursors();
	_tuioClient->lockCursorList();

	// Send "button" up events for cursors that were down last frame, but are now up.
	std::vector<int> downLast;
	for (std::set<int>::iterator downLast_it = _cursorsDown.begin(); downLast_it!= _cursorsDown.end(); ++downLast_it ) {
		downLast.push_back(*downLast_it);
	}

	for (int i=0;i<downLast.size();i++) {
		bool stillDown = false;
		for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {
			TuioCursor *tcur = (*iter);
			if (tcur->getCursorID() == downLast[i]) {
				stillDown = true;
			}
		}
		if (!stillDown) {
            VRDataIndex event = VRButtonEvent::createValidDataIndex("Touch" + intToStr(downLast[i]) + "_Up", 0);
            event.addData("TouchID", downLast[i]);
            events.push_back(event);
			_cursorsDown.erase(downLast[i]);
		}
	}

	// Send "button" down events for cursors that are new and updated positions for all cursors
	for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);

		if (_cursorsDown.find(tcur->getCursorID()) == _cursorsDown.end()) {
            VRDataIndex event = VRButtonEvent::createValidDataIndex("Touch" + intToStr(tcur->getCursorID()) + "_Down", 1);
            event.addData("TouchID", tcur->getCursorID());
            event.addData("X", (float)_xScale*tcur->getX());
            event.addData("Y", (float)_yScale*tcur->getY());
            events.push_back(event);
			_cursorsDown.insert(tcur->getCursorID());
		}

		if (tcur->getMotionSpeed() > 0.0) {
            std::vector<float> pos;
            pos.push_back((float)_xScale*tcur->getX());
            pos.push_back((float)_yScale*tcur->getY());

            std::vector<float> npos;
            npos.push_back((float)tcur->getX());
            npos.push_back((float)tcur->getY());
            
            VRDataIndex event = VRCursorEvent::createValidDataIndex("Touch" + intToStr(tcur->getCursorID()) + "_Move", pos, npos);
            event.addData("TouchID", tcur->getCursorID());
            events.push_back(event);
		}

		// Can also access several other properties of cursors (speed, acceleration, path followed, etc.)
		//std::cout << "cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY()
		// << " " << tcur->getMotionSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;

		// This is how to access all the points in the path that a cursor follows:
		//std::list<TuioPoint> path = tuioCursor->getPath();
		//if (path.size() > 0) {
		//  TuioPoint last_point = path.front();
		//  for (std::list<TuioPoint>::iterator point = path.begin(); point!=path.end(); point++) {
		//    last_point.update(point->getX(),point->getY());
		//  }
		//}
	}

    for (int f = 0; f < events.size(); f++) {
        inputEvents->push(events[f].serialize());
    }
    
	_tuioClient->unlockCursorList();
}

VRInputDevice* VRTUIODevice::create(VRMainInterface* vrMain,
		VRDataIndex* config, const std::string& nameSpace) {
	int port = TUIO_PORT;
	if (config->exists("Port", nameSpace)) {
		port = config->getValue("Port", nameSpace);
	}
	double xs = 1.0;
	if (config->exists("XScaleFactor", nameSpace)) {
		xs = config->getValue("XScaleFactor", nameSpace);
	}
	double ys = 1.0;
	if (config->exists("YScaleFactor", nameSpace)) {
		ys = config->getValue("YScaleFactor", nameSpace);
	}

	return new VRTUIODevice(port, xs, ys);
}

};         // end namespace
