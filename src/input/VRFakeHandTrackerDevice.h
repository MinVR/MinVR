/**
 This file is part of the MinVR Open Source Project, which is developed and
 maintained collaboratively by the University of Minnesota and Brown University.
 
 Copyright (c) 2016 Regents of the University of Minnesota and Brown University.
 This software is distributed under the BSD-3 Clause license, which can be found
 at: MinVR/LICENSE.txt.
 
 Original Author(s) of this File:
	Dan Keefe, 2017, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef VRFAKEHANDTRACKERDEVICE_H
#define VRFAKEHANDTRACKERDEVICE_H

#include <config/VRDataIndex.h>
#include <config/VRDataQueue.h>
#include <input/VRInputDevice.h>
#include <main/VRFactory.h>
#include <math/VRMath.h>

namespace MinVR {

/** A "virtual input device" that converts mouse and keyboard events into
    6 Degree-of-Freedom tracker events.  This facilitates debugging
    VR programs on a desktop computer.  
 
    This UI has a good default mapping for controlling hand-held VR cursors via
    mouse and keyoard input.  The mouse and keyboard events are trapped and then
    turned into 6-DOF tracker events so that you can write your program as if it
    is a VR program listening for tracker move events but control it with your
    desktop computer.
 
    To move the tracker in a plane parallel to the filmplane, just move the
    mouse.  Hold the "T" key on the keyboard to enter a translate-in-Z-direction
    mode, which will map mouse movement to movement perpendicular to the
    filmplane.  Hold the "R" key to rotate the tracker with the mouse.  Pressing
    the "2" key on the keyboard, toggles the entire interface on/off.
 
    Finally, note that all of the keys mentioned above are just defaults.  These
    can be reconfigured in the config file where the VRFakeHeadTrackerDevice is
    defined.  Scale factors can also be set in the config file to adjust the
    sensativity of the mouse-to-tracker movements.  The scale factors are 
    designed so that a default value of 1.0 works well on most machines.
  */
class VRFakeHandTrackerDevice : public VRInputDevice, public VREventHandler {
public:
    
    VRFakeHandTrackerDevice(const std::string &trackerName,
                            const std::string &toggleOnOffEventName,
                            float xyScale,
                            float zScale,
                            float rotScale,
                            std::vector<std::string> zKeys,
                            std::vector<std::string> rotKeys);
    
    virtual ~VRFakeHandTrackerDevice();
    
    void onVREvent(const VRDataIndex &eventData);

    void appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents);

    static VRInputDevice* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);
    
private:
    
    std::string _eventName;
    std::string _toggleEvent;
    float _xyScale;
    float _zScale;
    float _rScale;
    std::vector<std::string> _zKeys;
    std::vector<std::string> _rotKeys;

    
    enum TrackingState {
        XYTranslating,
        ZTranslating,
        Rotating,
    };
    
    TrackingState _state;
    bool _tracking;
    float _z;
    VRMatrix4 _R;
    float _lastMouseX, _lastMouseY;
    
    VRDataQueue _pendingEvents;
};
    
} // end namespace

#endif


