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

#ifndef VRFAKEHEADTRACKERDEVICE_H
#define VRFAKEHEADTRACKERDEVICE_H

#include <config/VRDataIndex.h>
#include <config/VRDataQueue.h>
#include <input/VRInputDevice.h>
#include <main/VRFactory.h>
#include <math/VRMath.h>

namespace MinVR {

/** A "virtual input device" that converts mouse and keyboard events into
    6 Degree-of-Freedom tracker events.  This facilitates debugging
    VR programs on a desktop computer.  
 
    This UI acts like a first person camera controller, but instead of setting
    camera parameters directly, it moves a fake VR user's HeadMatrix based on
    keyboard and mosue input.  Like typical first-person controllers in games,
    use the arrow keys or A,S,W,Z keys to move around in the virtual world.
    UP/DOWN and W/Z move forward and back.  LEFT/RIGHT or A/S rotate your body
    to face left or right.  Clicking and dragging with the right mouse button
    makes you look around, similar to the mouse-based head movement in games 
    like minecraft.  Pressing the "1" key on the keyboard, toggles the entire
    interface on/off.  
 
    Finally, note that all of the keys mentioned above are just defaults.  These
    can be reconfigured in the config file where the VRFakeHeadTrackerDevice is
    defined.  Scale factors can also be set in the config file to adjust the
    sensativity of the mouse-to-tracker movements.  The scale factors are
    designed so that a default value of 1.0 works well on most machines.
 */
class VRFakeHeadTrackerDevice : public VRInputDevice, public VREventHandler {
public:
    
    VRFakeHeadTrackerDevice(const std::string &trackerName,
                            const std::string &toggleOnOffEventName,
                            float tScale,
                            float rScale,
                            VRMatrix4 initialHeadFrame,
                            std::vector<std::string> forwardKeys,
                            std::vector<std::string> backKeys,
                            std::vector<std::string> leftKeys,
                            std::vector<std::string> rightKeys,
                            std::vector<std::string> mouseRotKeys);
    
    virtual ~VRFakeHeadTrackerDevice();
    
    void onVREvent(const VRDataIndex &eventData);

    void appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents);

    static VRInputDevice* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);
    
private:
    
    bool _tracking;
    std::string _eventName;
    std::string _toggleEvent;
    float _tScale;
    float _rScale;
    VRMatrix4 _baseHead;
    VRMatrix4 _addedRot;
    VRDataQueue _pendingEvents;
    float _yaw;
    float _pitch;
    bool _mouseRotating;
    float _lastMouseX;
    float _lastMouseY;
    
    std::vector<std::string> _forwardEvents;
    std::vector<std::string> _backEvents;
    std::vector<std::string> _leftEvents;
    std::vector<std::string> _rightEvents;
    std::vector<std::string> _mouseRotKeys;
};
    
} // end namespace

#endif


