
#include "VRFakeTrackerDevice.h"
#include <math/VRMath.h>

namespace MinVR {



VRFakeTrackerDevice::VRFakeTrackerDevice(const std::string &trackerName,
                                         const std::string &toggleOnOffEventName,
                                         float xyScale,
                                         float zScale,
                                         float rotScale)
{
    _eventName = trackerName + "_Move";
    _toggleEvent = toggleOnOffEventName;
    _xyScale = xyScale;
    _zScale = zScale;
    _rScale = rotScale;

    _tracking = true;
    _state = VRFakeTrackerDevice::XYTranslating;
    _z = 0.0;
}



VRFakeTrackerDevice::~VRFakeTrackerDevice()
{
}


void VRFakeTrackerDevice::onVREvent(const VRDataIndex &eventData)
{
    if (eventData.getName() == _toggleEvent) {
        _tracking = !_tracking;
        if (_tracking) {
            _state = VRFakeTrackerDevice::XYTranslating;
        }
    }
    else if (eventData.getName() == "KbdZ_Down") {
        _state = VRFakeTrackerDevice::ZTranslating;
    }
    else if (eventData.getName() == "KbdZ_Up") {
        _state = VRFakeTrackerDevice::XYTranslating;
    }
    else if (eventData.getName() == "KbdR_Down") {
        _state = VRFakeTrackerDevice::Rotating;
    }
    else if (eventData.getName() == "KbdR_Up") {
        _state = VRFakeTrackerDevice::XYTranslating;
    }
    else if (eventData.getName() == "Mouse_Move") {
        VRFloatArray screenPos = eventData.getValue("NormalizedPosition");
        if (screenPos.size() >= 2) {
            float mousex = 2.0*(screenPos[0] - 0.5);
            float mousey = 2.0*((1.0-screenPos[1]) - 0.5);

            if (_tracking) {
                float deltaX = mousex - _lastMouseX;
                float deltaY = mousey - _lastMouseY;

                if (_state == VRFakeTrackerDevice::ZTranslating) {
                    _z += _zScale * deltaY;
                }
                else if (_state == VRFakeTrackerDevice::Rotating) {
                    _R = VRMatrix4::rotationY(_rScale*deltaX) * VRMatrix4::rotationX(-_rScale*deltaY) * _R;
                }

                VRVector3 pos = VRVector3(_xyScale * mousex, _xyScale * mousey, _z);
                VRMatrix4 xform  = VRMatrix4::translation(pos) * _R;

                VRDataIndex di(_eventName);
                di.addData("Transform", xform);
                _pendingEvents.push_back(di);
            }

            _lastMouseX = mousex;
            _lastMouseY = mousey;
        }
    }
}


void VRFakeTrackerDevice::appendNewInputEventsSinceLastCall(VRDataQueue* queue)
{
    for (std::vector<VRDataIndex>::iterator evt = _pendingEvents.begin();
         evt < _pendingEvents.end(); ++evt) {
      queue->push(evt->serialize());
    }
    _pendingEvents.clear();
}


VRInputDevice*
VRFakeTrackerDevice::create(VRMainInterface *vrMain, VRDataIndex *config,
                            const std::string &nameSpace) {
  std::string devNameSpace = nameSpace;

  std::string trackerName = config->getValue("TrackerName", devNameSpace);
  std::string toggleEvent = config->getValue("ToggleOnOffEvent", devNameSpace);
  float xyScale = config->getValue("XYTranslationScale", devNameSpace);
  float zScale = config->getValue("ZTranslationScale", devNameSpace);
  float rScale = config->getValue("RotationScale", devNameSpace);

  VRFakeTrackerDevice *dev = new VRFakeTrackerDevice(trackerName, toggleEvent, xyScale, zScale, rScale);
  vrMain->addEventHandler(dev);

  return dev;
}


} // end namespace


