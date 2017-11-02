/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and 
maintained collaboratively by the University of Minnesota's Interactive 
Visualization Lab and the Brown University Visualization Research Lab.

File: VRVRPNButtonDevice.h

Original Author(s) of this File: 
	Daniel Keefe, 2004, Brown University (originally VRG3D/VRPNButtonDevice.H)
	
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
   \author     Daniel Keefe (dfk)
   \maintainer Daniel Keefe (dfk)
   
   \created 2004-01-28
   \edited  2004-01-28
   
   \file  VRPNButtonDevice.H
   
   \brief A driver that connects to VRPN and creates Events
          based on button data from VRPN.
*/

#ifndef VRPNBUTTONDEVICE_H
#define VRPNBUTTONDEVICE_H

// Note: This include ordering is important!
#include <vector>
#include <iostream>

#include <config/VRDataIndex.h>
#include <input/VRInputDevice.h>
#include <main/VRFactory.h>
#include <plugin/VRPlugin.h>

class vrpn_Button_Remote;

namespace MinVR {

/**
*/
class VRVRPNButtonDevice : public VRInputDevice
{
public:
	PLUGIN_API VRVRPNButtonDevice(const std::string &vrpnAnalogDeviceName, const std::vector<std::string> &eventsToGenerate);
	PLUGIN_API virtual ~VRVRPNButtonDevice();

	PLUGIN_API void        appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents);
	PLUGIN_API void        sendEvent(int buttonNumber, bool down);
	PLUGIN_API std::string getEventName(int buttonNumber);
	PLUGIN_API size_t      numButtons() { return _eventNames.size(); }
	PLUGIN_API static VRInputDevice* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

private:
	vrpn_Button_Remote        *_vrpnDevice;
	std::vector<std::string>   _eventNames;
    std::vector<VRDataIndex>   _pendingEvents;
};




} // end namespace

#endif




