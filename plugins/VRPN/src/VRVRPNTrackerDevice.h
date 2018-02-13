/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and 
maintained collaboratively by the University of Minnesota's Interactive 
Visualization Lab and the Brown University Visualization Research Lab.

File: VRVRPNTrackerDevice.h

Original Author(s) of this File: 
	Daniel Keefe, 2004, Brown University (originally VRG3D/VRPNTrackerDevice.H)
	
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

/**
\author Daniel Keefe (dfk)
\file   VRPNTrackerDevice.H
\brief  A driver that connects to VRPN and creates CoordinateFrame Events
based on tracker data from VRPN.
*/

#ifndef VRPNTRACKERDEVICE_H
#define VRPNTRACKERDEVICE_H

// Note: This include ordering is important!
#include <vector>
#include <iostream>

#include <config/VRDataIndex.h>
#include <input/VRInputDevice.h>
#include <main/VRFactory.h>
#include <math/VRMath.h>
#include <plugin/VRPlugin.h>

class vrpn_Tracker_Remote;
class vrpn_Connection;

namespace MinVR {

	/**
	Calibration Procedure:

	1. Set TrackerUnitsToRoomUnitsScale to get everything in feet.

	2. Adjust DeviceToRoom by printing out the position of sensor 0
	until it reports the origin of RoomSpace in the correct place and
	+X, +Y, and +Z all point in the correct direction.  You can print
	out the value of sensor 0 by calling printSensor0(true).

	3. Use the TestTrackers function of the IS3D/programTemplate demo
	to draw the CoordinateFrame for each tracker.  Adjust each
	tracker's PropToTracker transformation until it aligns itself with
	the RoomSpace frame when the tracker is held at the origin of
	RoomSpace.

	Here's an easy way to think about it: Hold up each prop at the
	origin of RoomSpace in the orientation that you want to be
	considered "normal".  Then, look at the drawing of its frame.  For
	each of the vectors in its frame (X,Y,Z) that are shown, ask
	yourself, what is the RoomSpace vector that this arrow depicts?
	Then enter that value in the appropriate X,Y, or Z column in the
	PropToTracker frame.

	4. For some of the props you may want the origin for the prop to
	be somewhere on the prop other than where the tracker is
	positioned.  For these, measure the distance from the tracker to
	where you want the origin to be, call this vector V.  Then, put
	this vector into the translation part of the PropToTracker frame.
	Be careful, the translation is applied after the rotation
	specified in the frame, so for example, if the first column of the
	frame is (0,1,0,0), you should put the Y component of V in that
	column, so it would become (0,1,0,Y[1]).  If it were (0,-1,0,0),
	you would need to put (0,-1,0,-Y[1]).
	*/
class VRVRPNTrackerDevice : public VRInputDevice
{
public:
	PLUGIN_API VRVRPNTrackerDevice(
		const std::string							   &vrpnTrackerDeviceName,
		const std::vector<std::string>				   &eventsToGenerate,
		const float								   &trackerUnitsToRoomUnitsScale,
		const VRMatrix4								   &deviceToRoom,
		const std::vector<VRMatrix4>			       &propToTracker,
		const std::vector<VRMatrix4>				   &finalOffset,
		const bool									   &waitForNewReportInPoll,
		const bool									   &convertLHtoRH = false,
		const bool									   &ignoreZeroes = false);

	PLUGIN_API virtual ~VRVRPNTrackerDevice();

	PLUGIN_API void processEvent(const VRMatrix4 &vrpnEvent, int sensorNum);
	PLUGIN_API std::string getEventName(int trackerNumber);
	PLUGIN_API void appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents);
	PLUGIN_API void setPrintSensor0(bool b) { _printSensor0 = b; }

	PLUGIN_API static VRInputDevice* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

private:
	vrpn_Tracker_Remote      *_vrpnDevice;
	std::vector<std::string>  _eventNames;
	float                    _trackerUnitsToRoomUnitsScale;
	VRMatrix4                 _deviceToRoom;
	std::vector<VRMatrix4>    _propToTracker;
	std::vector<VRMatrix4>    _finalOffset;
	bool                      _printSensor0;
	bool                      _waitForNewReport;
	bool                      _convertLHtoRH;
	bool                      _ignoreZeroes;
	bool                      _newReportFlag;
    std::vector<VRDataIndex>  _pendingEvents;
};


} // end namespace

#endif


