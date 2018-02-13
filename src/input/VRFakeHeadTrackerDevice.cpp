
#include "VRFakeHeadTrackerDevice.h"
#include <api/VRTrackerEvent.h>
#include <math/VRMath.h>

namespace MinVR {

    
    
VRFakeHeadTrackerDevice::VRFakeHeadTrackerDevice(const std::string &trackerName,
                                         const std::string &toggleOnOffEventName,
                                         float tScale,
                                         float rScale,
                                         VRMatrix4 initialHeadFrame,
                                         std::vector<std::string> forwardKeys,
                                         std::vector<std::string> backKeys,
                                         std::vector<std::string> leftKeys,
                                         std::vector<std::string> rightKeys,
                                         std::vector<std::string> mouseRotKeys)
{
    _eventName = trackerName + "_Move";
    _toggleEvent = toggleOnOffEventName;
    _tScale = tScale;
    _rScale = rScale;
    _baseHead = initialHeadFrame;
    _tracking = true;
    _yaw = 0.0;
    _pitch = 0.0;
    _mouseRotKeys = mouseRotKeys;
    _mouseRotating = false;
    
    for (std::vector<std::string>::iterator it = forwardKeys.begin(); it < forwardKeys.end(); ++it) {
        _forwardEvents.push_back(*it + "_Down");
        _forwardEvents.push_back(*it + "_Repeat");
    }
    for (std::vector<std::string>::iterator it = backKeys.begin(); it < backKeys.end(); ++it) {
        _backEvents.push_back(*it + "_Down");
        _backEvents.push_back(*it + "_Repeat");
    }
    for (std::vector<std::string>::iterator it = leftKeys.begin(); it < leftKeys.end(); ++it) {
        _leftEvents.push_back(*it + "_Down");
        _leftEvents.push_back(*it + "_Repeat");
    }
    for (std::vector<std::string>::iterator it = rightKeys.begin(); it < rightKeys.end(); ++it) {
        _rightEvents.push_back(*it + "_Down");
        _rightEvents.push_back(*it + "_Repeat");
    }
}
    


VRFakeHeadTrackerDevice::~VRFakeHeadTrackerDevice()
{
}


    
bool myEventMatch(std::string eventName, std::vector<std::string> keys, std::string state) {
    for (std::vector<std::string>::iterator it = keys.begin(); it < keys.end(); ++it) {
        if (*it + "_" + state == eventName) {
            return true;
        }
    }
    return false;
}
    
void VRFakeHeadTrackerDevice::onVREvent(const VRDataIndex &event)
{
    if (event.getName() == _toggleEvent) {
        _tracking = !_tracking;
    }
    
    if (_tracking) {
        bool sendEvent = false;
        if (std::find(_forwardEvents.begin(), _forwardEvents.end(), event.getName()) != _forwardEvents.end()) {
            VRMatrix4 M = _baseHead * VRMatrix4::rotationY(-_yaw);
            VRVector3 dir = M * VRVector3(0,0,-1);
            dir = dir.normalize();
            _baseHead = _baseHead * VRMatrix4::translation(0.075f * _tScale * dir);
            sendEvent = true;
        }
        else if (std::find(_backEvents.begin(), _backEvents.end(), event.getName()) != _backEvents.end()) {
            VRMatrix4 M = _baseHead * VRMatrix4::rotationY(-_yaw);
            VRVector3 dir = M * VRVector3(0,0,1);
            dir = dir.normalize();
            _baseHead = _baseHead * VRMatrix4::translation(0.075f * _tScale * dir);
            sendEvent = true;
        }
        else if (std::find(_leftEvents.begin(), _leftEvents.end(), event.getName()) != _leftEvents.end()) {
            _baseHead = _baseHead *  VRMatrix4::rotationY(0.025f * _rScale);
            sendEvent = true;
        }
        else if (std::find(_rightEvents.begin(), _rightEvents.end(), event.getName()) != _rightEvents.end()) {
            _baseHead = _baseHead * VRMatrix4::rotationY(-0.025f * _rScale);
            sendEvent = true;
        }
        else if (myEventMatch(event.getName(), _mouseRotKeys, "Down")) {
            _mouseRotating = true;
        }
        else if (myEventMatch(event.getName(), _mouseRotKeys, "Up")) {
            _mouseRotating = false;
        }
        else if (event.getName() == "Mouse_Move") {
            VRFloatArray screenPos = event.getValue("NormalizedPosition");
            if (screenPos.size() >= 2) {
                float mousex = 2.0f*(screenPos[0] - 0.5f);
                float mousey = 2.0f*((1.0f-screenPos[1]) - 0.5f);
                
                if (_mouseRotating) {
                    float deltaX = mousex - _lastMouseX;
                    float deltaY = mousey - _lastMouseY;
                    
                    _yaw += deltaX;
                    _pitch += deltaY;
                                        
                    _addedRot = VRMatrix4::rotationY(-_yaw) * VRMatrix4::rotationX(_pitch);
                    sendEvent = true;
                }
                
                _lastMouseX = mousex;
                _lastMouseY = mousey;
            }
        }
        
        if (sendEvent) {
            VRMatrix4 M = _baseHead * _addedRot;
            VRDataIndex di = VRTrackerEvent::createValidDataIndex(_eventName, M.toVRFloatArray());
            _pendingEvents.push(di);
        }
    }
}

    
void VRFakeHeadTrackerDevice::appendNewInputEventsSinceLastCall(VRDataQueue* inputEvents)
{
    inputEvents->addQueue(_pendingEvents);
    _pendingEvents.clear();
}
 
    

VRInputDevice*
VRFakeHeadTrackerDevice::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
    std::string devNameSpace = nameSpace;
  
    std::string trackerName = config->getValue("TrackerName", devNameSpace);
    std::string toggleEvent = config->getValueWithDefault("ToggleOnOffEvent", std::string("Kbd1_Down"), devNameSpace);
    float tScale = config->getValueWithDefault("TranslationScale", 1.0f, devNameSpace);
    float rScale = config->getValueWithDefault("RotationScale", 1.0f, devNameSpace);
    
    VRMatrix4 headMatrix = VRMatrix4::translation(VRVector3(0,0,4));
    if (config->exists("HeadMatrix", devNameSpace)){
        headMatrix = config->getValue("HeadMatrix", devNameSpace);
    }
    else if (config->exists("LookAtUp", devNameSpace) && config->exists("LookAtEye", devNameSpace) && config->exists("LookAtCenter", devNameSpace))
    {
        VRVector3 up = config->getValue("LookAtUp", devNameSpace);
        VRVector3 eye = config->getValue("LookAtEye", devNameSpace);
        VRVector3 center = config->getValue("LookAtCenter", devNameSpace);
        
        VRVector3 z = eye - center;
        z = z.normalize();
        VRVector3 x = up.cross(z);
        x = x.normalize();
        VRVector3 y = z.cross(x);
        
        VRMatrix4 M1 = VRMatrix4::fromRowMajorElements(x[0], y[0], z[0], 0,
                                                       x[1], y[1], z[1], 0,
                                                       x[2], y[2], z[2], 0,
                                                       0, 0, 0, 1);
        
        VRMatrix4 M2 = VRMatrix4::fromRowMajorElements(1, 0, 0, -eye[0],
                                                       0, 1, 0, -eye[1],
                                                       0, 0, 1, -eye[2],
                                                       0, 0, 0, 1);
        
        VRMatrix4 lookAtMatrix = M1 * M2;
        headMatrix = lookAtMatrix.inverse();
    }
    else {
        std::cerr << "Warning : no HeadMatrix defined for " << devNameSpace << std::endl;
        std::cerr << "Either Define HeadMatrix or LookAtUp, LookAtEye and LookAtCenter" << std::endl;
        std::cerr << "Using default: " << headMatrix << std::endl;
    }
    
    std::vector<std::string> forwardKeys;
    forwardKeys.push_back("KbdUp");
    forwardKeys.push_back("KbdW");
    forwardKeys = config->getValueWithDefault("ForwardKeys", forwardKeys, devNameSpace);
    
    std::vector<std::string> backKeys;
    backKeys.push_back("KbdDown");
    backKeys.push_back("KbdZ");
    backKeys = config->getValueWithDefault("BackKeys", backKeys, devNameSpace);
    
    std::vector<std::string> leftKeys;
    leftKeys.push_back("KbdLeft");
    leftKeys.push_back("KbdA");
    leftKeys = config->getValueWithDefault("LeftKeys", leftKeys, devNameSpace);
    
    std::vector<std::string> rightKeys;
    rightKeys.push_back("KbdRight");
    rightKeys.push_back("KbdS");
    rightKeys = config->getValueWithDefault("RightKeys", rightKeys, devNameSpace);

    std::vector<std::string> mouseRotKeys;
    mouseRotKeys.push_back("MouseBtnRight");
    mouseRotKeys = config->getValueWithDefault("MouseRotationKeys", mouseRotKeys, devNameSpace);
    
    VRFakeHeadTrackerDevice *dev = new VRFakeHeadTrackerDevice(trackerName, toggleEvent, tScale, rScale,
        headMatrix, forwardKeys, backKeys, leftKeys, rightKeys, mouseRotKeys);
    vrMain->addEventHandler(dev);

    return dev;
}

  
} // end namespace


