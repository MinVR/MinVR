/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and
maintained collaboratively by the University of Minnesota's Interactive
Visualization Lab and the Brown University Visualization Research Lab.

File: VRVRPNTrackerDevice.cpp

Original Author(s) of this File:
	Daniel Keefe, 2004, Brown University (originally VRG3D/VRPNTrackerDevice.cpp)

Author(s) of Significant Updates/Modifications to the File:
	Bret Jackson, 2013, University of Minnesota (adapted to MinVR)
	Dan Keefe, 2016, University of Minnesota (adapted to MinVR2)

-----------------------------------------------------------------------------------
Copyright (c) 2008-2015 Regents of the University of Minnesota and Brown University
Copyright (c) 2004-2008 Brown University
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of the University of Minnesota, Brown University, nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================ */


// Note: This include ordering is important!
#include "VRVRPNTrackerDevice.h"

#include <vrpn_Tracker.h>
#include <quat.h>

#include <math/VRMath.h>
#include <api/VRTrackerEvent.h>

#include <iostream>
using namespace std;

namespace MinVR {


#ifndef VRPN_CALLBACK
#define VRPN_CALLBACK
#endif


// Callback function for VRPN, void* pointer points to a VRPNTrackerDevice
void VRPN_CALLBACK trackerHandler(void *thisPtr, const vrpn_TRACKERCB info)
{
    double rotraw[16];
    q_to_ogl_matrix(rotraw, info.quat);
    float rotrawf[16];
    for (int i=0; i<16; i++) {
        rotrawf[i] = (float)rotraw[i];
    }
    VRMatrix4 vrpnEvent(rotrawf);
    vrpnEvent(0,3) = (float)(info.pos[0]);
    vrpnEvent(1,3) = (float)(info.pos[1]);
    vrpnEvent(2,3) = (float)(info.pos[2]);

	VRVRPNTrackerDevice* device = ((VRVRPNTrackerDevice*)thisPtr);
	device->processEvent(vrpnEvent, info.sensor);
}

VRVRPNTrackerDevice::VRVRPNTrackerDevice(
	const std::string              &vrpnTrackerDeviceName,
	const std::vector<std::string> &eventsToGenerate,
	const float                   &trackerUnitsToRoomUnitsScale,
	const VRMatrix4                &deviceToRoom,
	const std::vector<VRMatrix4>   &propToTracker,
	const std::vector<VRMatrix4>   &finalOffset,
	const bool                     &waitForNewReportInPoll,
	const bool                     &convertLHtoRH,
	const bool                     &ignoreZeroes)
{
	_eventNames                   = eventsToGenerate;
	_trackerUnitsToRoomUnitsScale = trackerUnitsToRoomUnitsScale;
	_deviceToRoom                 = deviceToRoom;
	_propToTracker                = propToTracker;
	_finalOffset                  = finalOffset;
	_waitForNewReport             = waitForNewReportInPoll;
	_convertLHtoRH                = convertLHtoRH;
	_ignoreZeroes                 = ignoreZeroes;
	_printSensor0                 = false;

	_vrpnDevice = new vrpn_Tracker_Remote(vrpnTrackerDeviceName.c_str());
	if (!_vrpnDevice)
	{
		std::stringstream ss;
		ss << "Can't create VRPN Remote Tracker with name " + vrpnTrackerDeviceName;
		std::cout << ss.str() << std::endl;
		//MinVR::Logger::getInstance().assertMessage(_vrpnDevice, ss.str().c_str());
	}
	_vrpnDevice->register_change_handler(this, trackerHandler);
}





VRVRPNTrackerDevice::~VRVRPNTrackerDevice()
{
}

/**
The tracker reports the transformation that would transform stuff
in tracker space to device space (that is, to the space relative to
the origin of the tracking device).  First we change the units on
the translation part of this since we usually work in feet and most
tracking systems report mesurements in some metric system.  Then we
have a transformation in feet relative to the device.  We multiply
by a transformation that transforms stuff in device space to IS3D
room space.  You can think of this as what rotation, then
translation would move the origin of RoomSpace to the origin of
tracking device.  This is the deviceToRoom coordinate frame.
*/
void VRVRPNTrackerDevice::processEvent(const VRMatrix4 &vrpnEvent, int sensorNum)
{

    if ((_ignoreZeroes) && (vrpnEvent.getColumn(3) == VRVector3(0,0,0))) {
    	return;
	}
	_newReportFlag = true;

	// first, adjust units of trackerToDevice.  after this, everything
	// is in RoomSpace units (typically feet for VRG3D).
	VRMatrix4 trackerToDevice = vrpnEvent;

	// convert a left handed coordinate system to a right handed one
	// not sure if this actually works..
	if (_convertLHtoRH) {
		/*** This is code from syzygy..
		CoordinateFrame switchMat(Matrix3(1,0,0, 0,0,1, 0,-1,0), Vector3::zero());
		CoordinateFrame transMat(switchMat.inverse() * vrpnEvent.translation);
		CoordinateFrame rotMat(vrpnEvent.rotation, Vector3::zero());
		trackerToDevice = transMat * switchMat.inverse() * rotMat * switchMat;
		***/

		// This code is based on the article "Conversion of Left-Handed
		// Coordinates to Right-Handed Coordinates" by David Eberly,
		// available online:
		// http://www.geometrictools.com/Documentation/LeftHandedToRightHanded.pdf
		trackerToDevice(2,3) = -trackerToDevice(2,3);

		trackerToDevice(0,2) = -trackerToDevice(0,2);
		trackerToDevice(1,2) = -trackerToDevice(1,2);
        trackerToDevice(2,0) = -trackerToDevice(2,0);
		trackerToDevice(2,1) = -trackerToDevice(2,1);
	}

	trackerToDevice(0,3) *= _trackerUnitsToRoomUnitsScale;
	trackerToDevice(1,3) *= _trackerUnitsToRoomUnitsScale;
	trackerToDevice(2,3) *= _trackerUnitsToRoomUnitsScale;

	VRMatrix4 eventRoom = _finalOffset[sensorNum] * _deviceToRoom * trackerToDevice * _propToTracker[sensorNum];

	if ((_printSensor0) && (sensorNum == 0)) {
        VRVector3 translation = eventRoom.getColumn(3);
		std::cout << translation << std::endl;
	}

    std::string name = getEventName(sensorNum) + "_Move";
    VRDataIndex event = VRTrackerEvent::createValidDataIndex(name, eventRoom.toVRFloatArray());
    _pendingEvents.push_back(event);
}

std::string VRVRPNTrackerDevice::getEventName(int trackerNumber)
{
  if (trackerNumber >= (int)_eventNames.size())
		return std::string("VRPNTrackerDevice_Unknown_Event");
	else
		return _eventNames[trackerNumber];
}


void VRVRPNTrackerDevice::appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents)
{
  // If this poll routine isn't called fast enough then the UDP buffer can fill up and
  // the most recent tracker records will be dropped, introducing lag in the system.
  // A workaround suggested by the VRPN website is to keep calling mainloop() until you
  // get a new tracker report.  This should only really be an issue if your framerate
  // is low.
  if (_waitForNewReport) {
    _newReportFlag = false;
    while (!_newReportFlag) {
      _vrpnDevice->mainloop();
    }
  }
  else {
    _vrpnDevice->mainloop();
  }

  for (int f = 0; f < _pendingEvents.size(); f++) {
    inputEvents->push(_pendingEvents[f].serialize());
  }
  _pendingEvents.clear();
}


VRInputDevice*
VRVRPNTrackerDevice::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
  std::string devNameSpace = nameSpace;

  std::string vrpnName = config->getValue("VRPNDeviceName", devNameSpace);
  std::vector<std::string> eventsToGenerate = config->getValue("EventsToGenerate", devNameSpace);
  float scale = config->getValue("TrackerUnitsToRoomUnitsScale", devNameSpace);
  VRMatrix4 d2r = config->getValue("DeviceToRoom", devNameSpace);
  d2r = d2r.orthonormal();
  bool wait = ((int)config->getValue("WaitForNewReportInPoll", nameSpace)) == 1;
  bool convert = ((int)config->getValue("ConvertLHtoRH", nameSpace)) == 1;
  bool ignore = ((int)config->getValue("IgnoreZeroes", nameSpace)) == 1;


  std::vector<VRMatrix4> p2t;
  std::vector<VRMatrix4> fo;
  for (size_t  i = 0; i < eventsToGenerate.size(); i++) {
	  std::string trackerNameSpace = config->validateNameSpace(nameSpace) + eventsToGenerate[i] + "/";

    VRMatrix4 m = config->getValue("PropToTracker", trackerNameSpace);
    m = m.orthonormal();
    p2t.push_back(m);
    m = config->getValue("FinalOffset", trackerNameSpace);
    m = m.orthonormal();
    fo.push_back(m);
  }

  VRInputDevice *dev = new VRVRPNTrackerDevice(vrpnName, eventsToGenerate, scale,
                                               d2r, p2t, fo, wait, convert, ignore);
  return dev;
}


} // end namespace


