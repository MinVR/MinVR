/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and
maintained collaboratively by the University of Minnesota's Interactive
Visualization Lab and the Brown University Visualization Research Lab.

File: VRVRPNAnalogDevice.cpp

Original Author(s) of this File:
	Daniel Keefe, 2004, Brown University (originally VRG3D/VRPNAnalogDevice.cpp)

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
#include "VRVRPNAnalogDevice.h"
#include <vrpn_Analog.h>
#include <iostream>

#include <api/VRAnalogEvent.h>


#ifndef VRPN_CALLBACK
#define VRPN_CALLBACK
#endif

namespace MinVR {

void VRPN_CALLBACK analogHandler(void *thisPtr, const vrpn_ANALOGCB info)
{
  int lastchannel = info.num_channel;
  if (lastchannel > (int)((VRVRPNAnalogDevice*)thisPtr)->numChannels()) {
    lastchannel = (int)(((VRVRPNAnalogDevice*)thisPtr)->numChannels());
  }
  for (int i=0;i<lastchannel;i++) {
    ((VRVRPNAnalogDevice*)thisPtr)->sendEventIfChanged(i, (float)info.channel[i]);
  }
}

VRVRPNAnalogDevice::VRVRPNAnalogDevice(const std::string &vrpnAnalogDeviceName, const std::vector<std::string> &eventsToGenerate)
{
	_eventNames = eventsToGenerate;
	for (size_t i=0;i<_eventNames.size();i++) {
		_channelValues.push_back(0.0);
	}

	_vrpnDevice = new vrpn_Analog_Remote(vrpnAnalogDeviceName.c_str());
	if (!_vrpnDevice) {
		std::stringstream ss;
		ss << "Can't create VRPN Remote Analog with name" + vrpnAnalogDeviceName;
		std::cout << ss.str() << std::endl;
	}

	_vrpnDevice->register_change_handler(this, analogHandler);
}

VRVRPNAnalogDevice::~VRVRPNAnalogDevice()
{
}

std::string	VRVRPNAnalogDevice::getEventName(int channelNumber)
{
  if (channelNumber >= (int)_eventNames.size()) {
		return std::string("VRPNAnalogDevice_Unknown_Event");
	}
	else {
		return _eventNames[channelNumber];
	}
}

void VRVRPNAnalogDevice::sendEventIfChanged(int channelNumber, float data)
{
	if (_channelValues[channelNumber] != data) {
		//_pendingEvents.push_back(EventRef(new Event(_eventNames[channelNumber], data, nullptr, channelNumber, msg_time)));
        std::string name = _eventNames[channelNumber] + "_Update";
        VRDataIndex di = VRAnalogEvent::createValidDataIndex(name, data);
		_pendingEvents.push_back(di);
		_channelValues[channelNumber] = data;
	}
}

void VRVRPNAnalogDevice::appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents)
{
	_vrpnDevice->mainloop();
    for (int f = 0; f < _pendingEvents.size(); f++) {
        inputEvents->push(_pendingEvents[f].serialize());
    }
    _pendingEvents.clear();
}


VRInputDevice*
VRVRPNAnalogDevice::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {

	std::string vrpnName = config->getValue("VRPNDeviceName", nameSpace);
	std::vector<std::string> eventsToGenerate = config->getValue("EventsToGenerate", nameSpace);

	VRInputDevice *dev = new VRVRPNAnalogDevice(vrpnName, eventsToGenerate);
	return dev;
}


} // end namespace



