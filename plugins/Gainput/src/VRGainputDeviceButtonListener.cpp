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
*/

#include "VRGainputDeviceButtonListener.h"
#include <iostream>
#include <api/VRButtonEvent.h>
#include <api/VRAnalogEvent.h>

namespace MinVR {
  VRGainputDeviceButtonListener::VRGainputDeviceButtonListener(
    gainput::InputManager& manager, int index) : manager_(manager), index_(index) 
  {
  }

  bool VRGainputDeviceButtonListener::OnDeviceButtonBool(gainput::DeviceId deviceId, gainput::DeviceButtonId deviceButton, bool oldValue, bool newValue)
  {
    const gainput::InputDevice* device = manager_.GetDevice(deviceId);
    char buttonName[64] = "";
    device->GetButtonName(deviceButton, buttonName, 64);
    std::string state;
    
    if (oldValue == 0 && newValue == 1)
    {
      state = "_Down" ;
    }
    else if(oldValue == 1 && newValue == 0)
    {
      state = "_UP";
    }
    
    std::string event_name = device->GetTypeName()+ std::to_string(device->GetIndex()) + "_" + buttonName + state;
    VRDataIndex di = VRButtonEvent::createValidDataIndex(event_name, newValue);
    _events.push_back(di);
    
    // we are adding events to the MinVR general queue
    return false;
  }

  bool VRGainputDeviceButtonListener::OnDeviceButtonFloat(gainput::DeviceId deviceId, gainput::DeviceButtonId deviceButton, float oldValue, float newValue)
  {
    const gainput::InputDevice* device = manager_.GetDevice(deviceId);
    char buttonName[64] = "";
    device->GetButtonName(deviceButton, buttonName, 64);

    std::string event_name = device->GetTypeName() + std::to_string(device->GetIndex()) + "_" + buttonName;
    VRDataIndex di = VRAnalogEvent::createValidDataIndex(event_name, newValue);
    _events.push_back(di);
    
    
    // we are adding events to the MinVR general queue
    return true;
  }

  int VRGainputDeviceButtonListener::GetPriority() const
  {
    return index_;
  }
  
  std::deque<VRDataIndex>& VRGainputDeviceButtonListener::vrInputDeviceEvents()
  {
    return _events;
  }
  void VRGainputDeviceButtonListener::clearEvents()
  {
    _events.clear();
  }
}

