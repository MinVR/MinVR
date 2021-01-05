/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and
maintained collaboratively by the University of Minnesota's Interactive
Visualization Lab and the Brown University Visualization Research Lab.

File: VRGainputDevice.cpp

Original Author(s) of this File:
	Camilo Diaz, 2020, Brown University 

-----------------------------------------------------------------------------------
Copyright (c) 2016-2020 Brown University
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
#include "VRGainputDevice.h"

#include <iostream>

#include <api/VRAnalogEvent.h>

#include <api/MinVR.h>


namespace MinVR {


VRGainputDevice::VRGainputDevice(int windowWidth,int windowHeight, int numControllers)
{
	_manager = new gainput::InputManager();
	_manager->SetDisplaySize(windowWidth, windowHeight);
	for (int i = 0 ; i < numControllers;++i)
	{
		gainput::DeviceId _padId = _manager->CreateDevice<gainput::InputDevicePad>();
		_padIds.push_back(_padId);

	}
	
	myDeviceButtonListener = new VRGainputDeviceButtonListener(*_manager, 1);
	gainput::ListenerId myDeviceButtonListenerId = _manager->AddListener(myDeviceButtonListener);

}

VRGainputDevice::~VRGainputDevice()
{
	delete myDeviceButtonListener;
	delete _manager;
}

void VRGainputDevice::appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents)
{
	//updates the listener queue
	_manager->Update();
	
	//adds gainput events to minvr event queue
	std::deque<VRDataIndex>& queue =  myDeviceButtonListener->vrInputDeviceEvents();
	while (!queue.empty())
	{
		inputEvents->push(queue.front());
		queue.pop_front();
	}
	
}


VRInputDevice*
VRGainputDevice::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace)
{
	int windowHeight = config->exists("Height", nameSpace);
	int windowWidth = config->exists("Width", nameSpace);
	int numControllers = 1;
	if (config->exists("GamePadControllers", nameSpace))
	{
		numControllers = config->getValue("GamePadControllers", nameSpace);
	}
	
	

	VRInputDevice *dev = new VRGainputDevice(windowWidth,windowHeight, numControllers);
	return dev;
}

  gainput::InputManager* VRGainputDevice::getGainInputManager()
{
	return _manager;
}

	PLUGIN_API std::vector<gainput::DeviceId>& VRGainputDevice::getPaids()
	{
		return _padIds;
	}


} // end namespace



