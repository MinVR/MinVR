/**
 This file is part of the MinVR Open Source Project, which is developed and
 maintained collaboratively by the University of Minnesota and Brown University.
 
 Copyright (c) 2016 Regents of the University of Minnesota and Brown University.
 This software is distributed under the BSD-3 Clause license, which can be found
 at: MinVR/LICENSE.txt.
 
 Original Author(s) of this File:
	Dan Keefe, 2017, University of Minnesota
  Josh Tveite, 2017, Brown University
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef VRFAKETRACKERDEVICE_H
#define VRFAKETRACKERDEVICE_H

#include <api/VREvent.h>
#include <config/VRDataIndex.h>
#include <config/VRDataQueue.h>
#include <input/VRInputDevice.h>
#include <main/VRFactory.h>
#include <math/VRMath.h>

namespace MinVR {

/** A "virtual input device" that converts mouse and keyboard events into 6
    Degree-of-Freedom tracker events.  This facilitates debugging VR programs
    on a desktop computer.  You can set up any number of devices, with
    different names.  The events will be issued as <name>_Move events.  So if
    your fake tracker is called "Head", it will issue "Head_Move" events.  A
    keyboard event is nominated to turn on and off the various trackers.

    There are four different modes enabled: XY translate, Z translate, XY
    rotate and Z rotate.  While in a mode, mouse movements control the
    rotation or translation.

    The tracker is chosen and modes are set and unset using keyboard events,
    which are all configurable.  The keys can be configured to be toggles,
    where pressing the key enters the mode and pressing it again exits, or as
    states, where the mode is only enabled while the key is pressed.  There
    are also scale factors you can set to adjust the sensitivity of the
    tracker to mouse movements.

    To operate in 'sticky' mode, press the toggleOnOff key, then use the
    translateEvent (default: 'w'), translateZEvent (default: 'z'),
    rotateEvent (default 'r'), and rollEvent (default: 'e') keys to change
    modes of operation.  The mouse will move the tracked object accordingly.
    To operate in non-sticky mode, hold down the relevant key while moving
    the mouse.
  */
class VRFakeTrackerDevice : public VRInputDevice, public VREventHandler {
public:
    
    VRFakeTrackerDevice(const std::string &trackerName,
                        const std::string &toggleOnOffEventName,
                        const std::string &rotateEventName,
                        const std::string &rollEventName,
                        const std::string &translateEventName,
                        const std::string &translateZEventName,
                        float xyScale,
                        float zScale,
                        float rotScale,
                        bool sticky,
                        VRVector3 startPos,
                        VRVector3 startDir,
                        VRVector3 startUp);
    
    virtual ~VRFakeTrackerDevice();
    
    void onVREvent(const VREvent &event);

    void appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents);

    static VRInputDevice* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);
    
private:

    std::string _eventName;

    // These contain the key names of the events that will trigger the given
    // behavior.
    std::string _toggleEvent;
    std::string _rotateOnEvent;
    std::string _rotateOffEvent;
    std::string _translateOnEvent;
    std::string _translateOffEvent;
    std::string _translateZOnEvent;
    std::string _translateZOffEvent;
    std::string _rollOnEvent;
    std::string _rollOffEvent;
    
    float _xyScale;
    float _zScale;
    float _rScale;

    // Are the key events toggles or on/off with down/up?
    bool _sticky;

    // These are the possible states of the tracked device.
    enum TrackingState {
        XYTranslating,
        ZTranslating,
        Rotating,
        Rolling,
        None
    };
    TrackingState _state;

    // Is the device currently listening for mouse input?
    bool _tracking;

    // The current state of affairs is given by the position vector and the
    // rotation matrix.  The _transform is the result of the two.
    VRMatrix4 _stateRot;
    VRVector3 _statePos;
    VRMatrix4 _transform;

    // We are mostly interested in the delta of the mouse movements, so we
    // keep track of the last measured location.
    float _lastMouseX, _lastMouseY;
    
    VRDataQueue _pendingEvents;
};
    
} // end namespace

#endif


