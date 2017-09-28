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

#ifndef VRFAKETRACKERDEVICERELATIVE_H
#define VRFAKETRACKERDEVICERELATIVE_H

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

The modes can be set and unset using keyboard
    events.  The keyboard events that control the tracker are all
    configurable.  The keys can be configured to be toggles, where pressing
    the key enters the mode and pressing it again exits, or as states, where
    the mode is only enabled while the key is pressed.  There are also scale
    factors you can set to adjust the sensitivity of the tracker to mouse
    movements.



Move the mouse to move the tracker
    in the XY-plane parallel to the scrren.  Hold z to move the tracker
    in/out of the screen.  Hold 'r' to rotate the tracker with the mouse.
    The scale factors set in the constructor 
  */
class VRFakeTrackerDeviceRelative : public VRInputDevice, public VREventHandler {
public:
    
    VRFakeTrackerDeviceRelative(const std::string &trackerName,
                        const std::string &toggleOnOffEventName,
                        const std::string &rotateEventName,
                        const std::string &rollEventName,
                        const std::string &translateEventName,
                        const std::string &translateZEventName,
                        float xyScale,
                        float zScale,
                        float rotScale,
                        bool rotateSticky,
                        bool rollSticky,
                        bool translateSticky,
                        bool translateZSticky,
                        VRVector3 startPos,
                        VRVector3 startDir,
                        VRVector3 startUp);
    
    virtual ~VRFakeTrackerDeviceRelative();
    
    void onVREvent(const VREvent &event);

    void appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents);

    static VRInputDevice* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);
    
private:
    
    std::string _eventName;
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
    bool _rotateSticky;
    bool _rollSticky;
    bool _translateSticky;
    bool _translateZSticky;
    
    enum TrackingState {
        XYTranslating,
        ZTranslating,
        Rotating,
        Rolling,
        None
    };
    
    TrackingState _state;
    bool _tracking;
    float _z;
    VRMatrix4 _R;
    VRVector3 _pos;
    VRMatrix4 _transform;
    float _lastMouseX, _lastMouseY;
    
    VRDataQueue _pendingEvents;
};
    
} // end namespace

#endif


