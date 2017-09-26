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

/** A "virtual input device" that converts mouse and keyboard events into
    6 Degree-of-Freedom tracker events.  This facilitates debugging
    VR programs on a desktop computer.  Move the mouse to move the tracker
    in the XY-plane parallel to the scrren.  Hold z to move the tracker
    in/out of the screen.  Hold 'r' to rotate the tracker with the mouse.
    The scale factors set in the constructor adjust the sensitivity of the
    tracker to each of these mouse movements.
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


