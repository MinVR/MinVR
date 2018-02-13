
#include "VRFakeHandTrackerDevice.h"
#include <api/VRTrackerEvent.h>
#include <math/VRMath.h>

namespace MinVR {

    
    
VRFakeHandTrackerDevice::VRFakeHandTrackerDevice(const std::string &trackerName,
                                         const std::string &toggleOnOffEventName,
                                         float xyScale,
                                         float zScale,
                                         float rotScale,
                                         std::vector<std::string> zKeys,
                                         std::vector<std::string> rotKeys)
{
    _eventName = trackerName + "_Move";
    _toggleEvent = toggleOnOffEventName;
    _xyScale = xyScale;
    _zScale = zScale;
    _rScale = rotScale;
    _zKeys = zKeys;
    _rotKeys = rotKeys;

    _tracking = true;
    _state = VRFakeHandTrackerDevice::XYTranslating;
    _z = 0.0;
}
    


VRFakeHandTrackerDevice::~VRFakeHandTrackerDevice()
{
}


bool eventMatch(std::string eventName, std::vector<std::string> keys, std::string state) {
    for (std::vector<std::string>::iterator it = keys.begin(); it < keys.end(); ++it) {
        if (*it + "_" + state == eventName) {
            return true;
        }
    }
    return false;
}
    

void VRFakeHandTrackerDevice::onVREvent(const VRDataIndex &event)
{
    if (event.getName() == _toggleEvent) {
        _tracking = !_tracking;
        if (_tracking) {
            _state = VRFakeHandTrackerDevice::XYTranslating;
        }
    }
    else if (eventMatch(event.getName(), _zKeys, "Down")) {
        _state = VRFakeHandTrackerDevice::ZTranslating;
    }
    else if (eventMatch(event.getName(), _zKeys, "Up")) {
        _state = VRFakeHandTrackerDevice::XYTranslating;
    }
    else if (eventMatch(event.getName(), _rotKeys, "Down")) {
        _state = VRFakeHandTrackerDevice::Rotating;
    }
    else if (eventMatch(event.getName(), _rotKeys, "Up")) {
        _state = VRFakeHandTrackerDevice::XYTranslating;
    }
    else if (event.getName() == "Mouse_Move") {
        VRFloatArray screenPos = event.getValue("NormalizedPosition");
        if (screenPos.size() >= 2) {
            float mousex = 2.0f*(screenPos[0] - 0.5f);
            float mousey = 2.0f*((1.0f-screenPos[1]) - 0.5f);

            if (_tracking) {
                float deltaX = mousex - _lastMouseX;
                float deltaY = mousey - _lastMouseY;
            
                if (_state == VRFakeHandTrackerDevice::ZTranslating) {
                    _z += _zScale * deltaY;
                }
                else if (_state == VRFakeHandTrackerDevice::Rotating) {
                    _R = VRMatrix4::rotationY(_rScale*deltaX) * VRMatrix4::rotationX(-_rScale*deltaY) * _R;
                }
            
                VRVector3 pos = VRVector3(_xyScale * mousex, _xyScale * mousey, _z);
                VRMatrix4 xform  = VRMatrix4::translation(pos) * _R;

                VRDataIndex di = VRTrackerEvent::createValidDataIndex(_eventName, xform.toVRFloatArray());
                _pendingEvents.push(di);
            }
            
            _lastMouseX = mousex;
            _lastMouseY = mousey;
        }
    }
}

    
void VRFakeHandTrackerDevice::appendNewInputEventsSinceLastCall(VRDataQueue* inputEvents)
{
    inputEvents->addQueue(_pendingEvents);
    _pendingEvents.clear();
}
 
    

VRInputDevice*
VRFakeHandTrackerDevice::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
    std::string devNameSpace = nameSpace;
  
    std::string trackerName = config->getValue("TrackerName", devNameSpace);
    std::string toggleEvent = config->getValueWithDefault("ToggleOnOffEvent", std::string("Kbd2_Down"), devNameSpace);
    float xyScale = config->getValueWithDefault("XYTranslationScale", 1.0f, devNameSpace);
    float zScale = config->getValueWithDefault("ZTranslationScale", 1.0f, devNameSpace);
    float rScale = config->getValueWithDefault("RotationScale", 1.0f, devNameSpace);
    
    std::vector<std::string> zKeys;
    zKeys.push_back("KbdT");
    zKeys = config->getValueWithDefault("ZTranslationKeys", zKeys);
    
    std::vector<std::string> rotKeys;
    rotKeys.push_back("KbdR");
    rotKeys = config->getValueWithDefault("RotationKeys", rotKeys);
    
    
    VRFakeHandTrackerDevice *dev = new VRFakeHandTrackerDevice(trackerName, toggleEvent, xyScale, zScale, rScale, zKeys, rotKeys);
    vrMain->addEventHandler(dev);

    return dev;
}

  
} // end namespace


