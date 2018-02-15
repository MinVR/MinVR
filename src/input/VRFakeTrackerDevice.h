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

#include <config/VRDataIndex.h>
#include <config/VRDataQueue.h>
#include <input/VRInputDevice.h>
#include <main/VRFactory.h>
#include <math/VRMath.h>

namespace MinVR {

/** A "virtual input device" that converts mouse and keyboard events into 6
    Degree-of-Freedom tracker events.  This facilitates debugging VR programs
    on a desktop computer.  You can set up any number of devices, with
    different names.  The events will be issued as "name"_Move events.  So if
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

    The rotations and translations happen in a 'Seeker' style, or a 'Looker'
    style.  In a head-tracked context, the 'Seeker' style is what you would
    use to explore a landscape, while the 'Looker' style is what you would
    use to look at an object hanging there in front of you.  When
    translating, the mouse movements move the position in the horizontal
    plane, more or less, or up and down.  In the 'Looker' style, the same
    mouse movements move the viewer in a vertical plane, or forward and
    back.  In the 'Seeker' style, the horizontal rotations spin the viewer,
    while in the 'Looker' style, they spin the viewer around the origin.

    To configure the VRFakeTrackerDevice, use a config file clause like this:
    \verbatim
    <FakeHead inputdeviceType="VRFakeTrackerDevice">
      <TrackerName>Head</TrackerName>
      <ToggleOnOffEvent>Kbdh_Down</ToggleOnOffEvent>
    </FakeHead>
    \endverbatim

    Within the clause, you can specify the following values (and
    defaults):

    RotateEvent(Kbdr), RollEvent(Kbde), TranslateEvent(Kbdw),
    TranslateZEvent(Kbdz), XYTranslationScale(1.0), ZTranslationScale(1.0),
    Sticky(0), Style(Looker).

    See the output of printInstruction() for the values in use.

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
                        bool seeker,
                        VRVector3 LookAtEye,
                        VRVector3 LookAtCenter,
                        VRVector3 LookAtUp);

    virtual ~VRFakeTrackerDevice();

    /// This is the actual event listener that takes keyboard events and
    /// translates them into tracker events.  Note that the original
    /// keyboard events are not devoured, so they are still available to
    /// whoever is listening to them.  The input is an event string.
    void onVREvent(const VRDataIndex &eventData);

    void appendNewInputEventsSinceLastCall(VRDataQueue* inputEvents);

    /// Produces a string of instructions about how to use this tracker, given
    /// all the input options.
    std::string printInstructions();

    static VRInputDevice* create(VRMainInterface *vrMain,
                                 VRDataIndex *config,
                                 const std::string &nameSpace);

private:

    /// The name of the tracker.  This is only useful for the output of
    /// printInstructions() and other self-identification purposes.
    std::string _trackerName;
    /// The actual name of the event the tracker will produce.
    std::string _eventName;

    /// The key name of the event that will toggle on and off the production
    /// of events by the fake tracker device.
    std::string _toggleEvent;
    /// The key name used to turn on rotating (X and Y) with the mouse.
    std::string _rotateOnEvent;
    /// The key name used to turn off rotating (X and Y) with the mouse.
    std::string _rotateOffEvent;
    /// The key name used to turn on translating (X and Y) with the mouse.
    std::string _translateOnEvent;
    /// The key name used to turn off translating (X and Y) with the mouse.
    std::string _translateOffEvent;
    /// The key name used to turn on translating (Z) with the mouse.
    std::string _translateZOnEvent;
    /// The key name used to turn off translating (Z) with the mouse.
    std::string _translateZOffEvent;
    /// The key name used to turn on rolling (rotate around Z) with the mouse.
    std::string _rollOnEvent;
    /// The key name used to turn off rolling (rotate around Z) with the mouse.
    std::string _rollOffEvent;

    /// A scale factor for translating in X and Y.
    float _xyScale;
    /// A scale factor for translating in Z.
    float _zScale;
    /// A scale for rotation in X,Y,Z.  Set this to pi to make one width of
    /// the screen equal to a full rotation.
    float _rScale;

    /// Are the key events toggles or on/off with down/up?  Set this to true to
    /// make the keys into toggles.
    bool _sticky;

    /// Which style of tracker device are we using? (Seeker or looker) A seeker is
    /// for looking around landscapes while a looker is for examining objects.
    bool _seeker;

    /// These are the possible states of the tracker device.
    enum TrackingState {
        XYTranslating,
        ZTranslating,
        Rotating,
        Rolling,
        None
    };

    /// The current state of the tracker is recorded here.
    TrackingState _state;

    /// Is the device currently listening for mouse input?
    bool _tracking;

    /// The current state of affairs is given by the position vector and the
    /// rotation matrix.  The _transform is the result of the two.
    VRVector3 _statePos;
    /// Current state of rotation.
    VRMatrix4 _stateRot;
    /// The matrix representation of _statePos and _stateRot.
    VRMatrix4 _transform;

    /// We are mostly interested in the delta of the mouse movements, so we
    /// keep track of the last measured location.
    float _lastMouseX, _lastMouseY;

    /// The queue of events we are planning to send.
    VRDataQueue _pendingEvents;
};

} // end namespace

#endif


