//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

/**
 * \author Daniel Keefe (dfk)
 *
 * \file  InputDevice.h
 *
 */


#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <G3D/G3D.h>
#include <GLG3D/GLG3D.h>

#include  "VRG3DEvent.h"
#include  "ConfigMap.h"

 /*
using namespace std;

namespace MinVR {

// Base class for InputDevices.  Input Devices should be polled once
//    per frame and append Events that were generated since the last
 //   call to poll to the array passed into the poll routine.


  class InputDevice
{
public:
  InputDevice() {}
  virtual ~InputDevice() {}

  virtual void pollForInput(G3D::Array<EventRef> &events) {}


  static  InputDevice  *newInputDevice( const std::string   name,
                                                   G3D::Log     *log,
                                        const ConfigMapRef  map );


  static  void  unsupportedDevice( const std::string   name,
                                              G3D::Log     *log,
                                   const std::string   variable );
};

}                 // end namespace VRG3D

#endif            // INPUTDEVICE_H
*/