
#include "VRFakeTrackerDevice.h"
#include <math/VRMath.h>
#include <api/VRTrackerEvent.h>

namespace MinVR {

VRFakeTrackerDevice::VRFakeTrackerDevice(const std::string &trackerName,
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
                                         VRVector3 startPos,
                                         VRVector3 startDir,
                                         VRVector3 startUp)
{
    _eventName = trackerName + "_Move";
    _toggleEvent = toggleOnOffEventName;
    _rotateOnEvent = rotateEventName + "_Down";
    _rotateOffEvent = rotateEventName + "_Up";
    _translateOnEvent = translateEventName + "_Down";
    _translateOffEvent = translateEventName + "_Up";
    _translateZOnEvent = translateZEventName + "_Down";
    _translateZOffEvent = translateZEventName + "_Up";
    _rollOnEvent = rollEventName + "_Down";
    _rollOffEvent = rollEventName + "_Up";
    _xyScale = xyScale;
    _zScale = zScale;
    _rScale = rotScale;
    _sticky = sticky;
    _seeker = seeker;

    _tracking = false;
    _state = VRFakeTrackerDevice::XYTranslating;

    _statePos = startPos;

    VRVector3 forward = startDir.normalize();
    VRVector3 x = startUp.cross(forward).normalize();
    VRVector3 up = forward.cross(x);
    std::cout << "StartDir" << startDir << std::endl;
    std::cout << "StartUp" << startUp << std::endl;
    std::cout << "StartPos" << startPos << std::endl;
    std::cout << "forward" << forward << std::endl;
    std::cout << "x" << x  << std::endl;
    std::cout << "up" << up  << std::endl;

    _stateRot = VRMatrix4::fromRowMajorElements( x[0], up[0], forward[0], 0,
                                                 x[1], up[1], forward[1], 0,
                                                 x[2], up[2], forward[2], 0,
                                                 0, 0, 0, 1);
    _transform = VRMatrix4::translation(_statePos) * _stateRot;
    VRMatrix4 xform  = _transform;

    VRDataIndex di(_eventName);
    di.addData(_eventName + "/Transform", xform);
    _pendingEvents.push(di);

}



VRFakeTrackerDevice::~VRFakeTrackerDevice()
{
}

void VRFakeTrackerDevice::onVREvent(const VRDataIndex &eventData)
{
  if (eventData.getName() == _toggleEvent) {
    _tracking = !_tracking;
    if (_tracking) {
      if (_sticky) {
        _state = VRFakeTrackerDevice::XYTranslating;
      } else {
        _state = VRFakeTrackerDevice::None;
      }
    }
  }
  else if (eventData.getName() == _translateZOnEvent) {
    if (_state !=  VRFakeTrackerDevice::ZTranslating) {
      _state = VRFakeTrackerDevice::ZTranslating;
    } else {
      _state = VRFakeTrackerDevice::None;
    }
  }
  else if (eventData.getName() == _translateOnEvent) {
    if (_state != VRFakeTrackerDevice::XYTranslating) {
      _state = VRFakeTrackerDevice::XYTranslating;
    } else {
      _state = VRFakeTrackerDevice::None;
    }
  }
  else if (eventData.getName() == _rotateOnEvent) {
    if (_state != VRFakeTrackerDevice::Rotating) {
      _state = VRFakeTrackerDevice::Rotating;
    } else {
      _state = VRFakeTrackerDevice::None;
    }
  }
  else if (eventData.getName() == _rollOnEvent) {
    if (_state != VRFakeTrackerDevice::Rolling) {
      _state = VRFakeTrackerDevice::Rolling;
    } else {
      _state = VRFakeTrackerDevice::None;
    }
  }
  else if (eventData.getName() == _translateZOffEvent && !_sticky) {
    _state = VRFakeTrackerDevice::None;
  }
  else if (eventData.getName() == _translateOffEvent && !_sticky) {
    _state = VRFakeTrackerDevice::None;
  }
  else if (eventData.getName() == _rotateOffEvent && !_sticky) {
    _state = VRFakeTrackerDevice::None;
  }
  else if (eventData.getName() == _rollOffEvent && !_sticky) {
    _state = VRFakeTrackerDevice::None;
  }
  else if (eventData.getName() == "Mouse_Move") {
    const VRFloatArray screenPos = eventData.getValue("NormalizedPosition");
    if (!screenPos.empty()) {

      // Transform range from [0,1] to [-1,1].
      float mousex = 2.0 * (screenPos[0] - 0.5);
      float mousey = 2.0 * ((1.0 - screenPos[1]) - 0.5);

      // If we're not currently tracking, ignore the mouse.
      if (_tracking) {
        float deltaX = mousex - _lastMouseX;
        float deltaY = mousey - _lastMouseY;

        if (_state == VRFakeTrackerDevice::ZTranslating) {
          // If we're Z translating, that's up and down in the seeker mode, but
          // forward and backward in the looker mode.
          if (_seeker) {
            _statePos =  VRVector3(0, _zScale * deltaY, 0) + _statePos;
          } else {
            _statePos =  VRVector3(0, 0, _zScale * deltaY) + _statePos;
          }
        }
        else if (_state == VRFakeTrackerDevice::Rotating) {
          // The seeker mode turns the viewer around in place, while the looker
          // mode rotates the object in front of the viewer.  More or less.

          VRMatrix4 r;

          if (_seeker) {
            VRVector3 up = _stateRot * VRVector3(0.0f, 1.0f, 0.0f);
            // Not sure why these coordinates have to be negated.
            VRPoint3 here = VRPoint3(-_statePos[0], -_statePos[1], -_statePos[2]);
            VRVector3 over = up.cross(_statePos);

            r = VRMatrix4::rotation(here, up, _rScale * deltaX) *
              VRMatrix4::rotation(here, over, _rScale * deltaY);

          } else {
            r = VRMatrix4::rotationY(_rScale * deltaX) *
              VRMatrix4::rotationX(-_rScale * deltaY);
          }
          _stateRot = r * _stateRot;
        }
        else if (_state == VRFakeTrackerDevice::Rolling) {
          VRMatrix4 r = VRMatrix4::rotationZ(_rScale * deltaX);
          _stateRot = r * _stateRot;
        }
        else if (_state == VRFakeTrackerDevice::XYTranslating){
          // The seeker mode moves us in the horizontal plane, while the looker
          // mode moves us in a vertical plane.
          if (_seeker) {
            _statePos =  _xyScale * VRVector3(deltaX, 0, deltaY) + _statePos;
          } else {
            _statePos =  _xyScale * VRVector3(deltaX, deltaY, 0) + _statePos;
          }
        }

        _transform = VRMatrix4::translation(_statePos) * _stateRot;

        VRDataIndex di(_eventName);
        di.addData(_eventName + "/Transform", _transform);
        _pendingEvents.push(di);
      }

      _lastMouseX = mousex;
      _lastMouseY = mousey;
    }
  }
}


void VRFakeTrackerDevice::appendNewInputEventsSinceLastCall(VRDataQueue* inputEvents)
{
  inputEvents->addQueue(_pendingEvents);
  _pendingEvents.clear();
}


VRInputDevice*
VRFakeTrackerDevice::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
    std::string devNameSpace = nameSpace;

    // Find and/or set all the default values.
    std::string trackerName = config->getValue("TrackerName", devNameSpace);
    std::string toggleEvent = config->getValue("ToggleOnOffEvent", devNameSpace);
    std::string rotateEvent = config->getValueWithDefault("RotateEvent",
                                                          std::string("Kbdr"),
                                                          devNameSpace);
    std::string translateEvent = config->getValueWithDefault("TranslateEvent",
                                                             std::string("Kbdw"),
                                                             devNameSpace);
    std::string translateZEvent = config->getValueWithDefault("TranslateZEvent",
                                                              std::string("Kbdz"),
                                                              devNameSpace);
    std::string rollEvent = config->getValueWithDefault("RollEvent",
                                                        std::string("Kbde"),
                                                        devNameSpace);
    float xyScale =
      config->getValueWithDefault("XYTranslationScale", 1.0f, devNameSpace);
    float zScale =
      config->getValueWithDefault("ZTranslationScale", 1.0f, devNameSpace);
    float rScale = config->getValueWithDefault("RotationScale",
                                               3.1415926f, devNameSpace);

    int sticky = config->getValueWithDefault("Sticky", 0, devNameSpace);
    std::string style = config->getValueWithDefault("Style",
                                                    std::string("Looker"),
                                                    devNameSpace);
    // We only have two styles, so we can use a boolean for it.  (Note that the
    // 'compare()' method returns a zero when the strings match.)
    bool seeker = !style.compare("Seeker");

    int pos[] = {0, 0, -1};
    int dir[] = {0, 0, 1};
    int up[] = {0, 1, 0};
    VRFloatArray defaultPos (pos, pos + sizeof(pos) / sizeof(pos[0]));
    VRFloatArray defaultDir (dir, dir + sizeof(dir) / sizeof(dir[0]));
    VRFloatArray defaultUp (up, up + sizeof(up) / sizeof(up[0]));
    VRVector3 startPos = config->getValueWithDefault("StartingPosition",
                                                     defaultPos, devNameSpace);
    VRVector3 startDir = config->getValueWithDefault("StartingDirection",
                                                     defaultDir, devNameSpace);
    VRVector3 startUp = config->getValueWithDefault("StartingUp",
                                                    defaultUp, devNameSpace);

    // Make a new object.
    VRFakeTrackerDevice *dev = new VRFakeTrackerDevice(trackerName,
                                                       toggleEvent,
                                                       rotateEvent,
                                                       rollEvent,
                                                       translateEvent,
                                                       translateZEvent,
                                                       xyScale,
                                                       zScale,
                                                       rScale,
                                                       sticky,
                                                       seeker,
                                                       startPos,
                                                       startDir,
                                                       startUp);
    vrMain->addEventHandler(dev);

    return dev;
}


} // end namespace


