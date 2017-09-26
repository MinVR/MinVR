
#include "VRFakeTrackerDeviceRelative.h"
#include <math/VRMath.h>

namespace MinVR {

    
    
/**  TODO:  Add NormalizedPosition to the Mouse_Move event
            Register VRFakeTrackerDeviceRelative with VRFactory
*/
    
    
    
VRFakeTrackerDeviceRelative::VRFakeTrackerDeviceRelative(const std::string &trackerName,
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
    _rotateSticky = rotateSticky;
    _rollSticky = rollSticky;
    _translateSticky = translateSticky;
    _translateZSticky = translateZSticky;


    _tracking = false;
    _state = VRFakeTrackerDeviceRelative::XYTranslating;
    _z = 0.0;

    _pos = startPos;

    VRVector3 forward = startDir.normalize();
    VRVector3 x = startUp.cross(forward).normalize();
    VRVector3 up = forward.cross(x);
    std::cout << "StartDir" << startDir << std::endl;
    std::cout << "StartUp" << startUp << std::endl;
    std::cout << "StartPos" << startPos << std::endl;
    std::cout << "forward" << forward << std::endl;
    std::cout << "x" << x  << std::endl;
    std::cout << "up" << up  << std::endl;

    _R = VRMatrix4::fromRowMajorElements( x[0], up[0], forward[0], 0,
                                          x[1], up[1], forward[1], 0,
                                          x[2], up[2], forward[2], 0,
                                          0, 0, 0, 1);
    _transform = VRMatrix4::translation(_pos) * _R;
    VRMatrix4 xform  = _transform;
    /*
    for (int i = 0; i < 4; i++){
      for (int j = 0; j < 4; j++){
        std::cout << setw(5) << xform(i,j) << " ";
      }
      std::cout << std::endl;
    }
    */
    VRDataIndex di;
    di.addData(_eventName + "/Transform", xform);
    _pendingEvents.push(di.serialize(_eventName));

}
    


VRFakeTrackerDeviceRelative::~VRFakeTrackerDeviceRelative()
{
}


void VRFakeTrackerDeviceRelative::onVREvent(const MinVR::VREvent &event)
{
    if (event.getName() == _toggleEvent) {
        _tracking = !_tracking;
        if (_tracking) {
            std::cout << "Starting Tracking" << std::endl;
            _state = VRFakeTrackerDeviceRelative::None;
        }
    }
    else if (event.getName() == _translateZOnEvent) {
        _state = VRFakeTrackerDeviceRelative::ZTranslating;
    }
    else if (event.getName() == _translateOnEvent) {
        _state = VRFakeTrackerDeviceRelative::XYTranslating;
    }
    else if (event.getName() == _rotateOnEvent) {
        _state = VRFakeTrackerDeviceRelative::Rotating;
    }
    else if (event.getName() == _rollOnEvent) {
        _state = VRFakeTrackerDeviceRelative::Rolling;
    }
    else if (event.getName() == _translateZOffEvent && !_translateZSticky) {
        _state = VRFakeTrackerDeviceRelative::None;
    }
    else if (event.getName() == _translateOffEvent && !_translateSticky) {
        _state = VRFakeTrackerDeviceRelative::None;
    }
    else if (event.getName() == _rotateOffEvent && !_rotateSticky) {
        _state = VRFakeTrackerDeviceRelative::None;
    }
    else if (event.getName() == _rollOffEvent && !_rollSticky) {
        _state = VRFakeTrackerDeviceRelative::None;
    }
    else if (event.getName() == "Mouse_Move") {
        const float *screenPos = event.getDataAsFloatArray("NormalizedPosition");
        if (screenPos != NULL) {
            float mousex = 2.0*(screenPos[0] - 0.5);
            float mousey = 2.0*((1.0-screenPos[1]) - 0.5);

            if (_tracking) {
                float deltaX = mousex - _lastMouseX;
                float deltaY = mousey - _lastMouseY;
            
                if (_state == VRFakeTrackerDeviceRelative::ZTranslating) {
                    _pos =  VRVector3(0, 0, _zScale * deltaY) + _pos;
                }
                else if (_state == VRFakeTrackerDeviceRelative::Rotating) {
                    VRMatrix4 r = VRMatrix4::rotationY(_rScale*deltaX) * VRMatrix4::rotationX(-_rScale*deltaY);
                    _R = r * _R;
                }
                else if (_state == VRFakeTrackerDeviceRelative::Rolling) {
                    VRMatrix4 r = VRMatrix4::rotationZ(_rScale*deltaX);
                    _R = r * _R;
                }
                else if (_state == VRFakeTrackerDeviceRelative::XYTranslating){
                    _pos =  _xyScale * VRVector3(deltaX, deltaY, 0) + _pos;
                } 
                _transform = VRMatrix4::translation(_pos) * _R;
                VRMatrix4 xform  = _transform;
                /*
                for (int i = 0; i < 4; i++){
                  for (int j = 0; j < 4; j++){
                    std::cout << setw(5) << xform(i,j) << " ";
                  }
                  std::cout << std::endl;
                }
                */
                VRDataIndex di;
                di.addData(_eventName + "/Transform", xform);
                _pendingEvents.push(di.serialize(_eventName));
            }
            
            _lastMouseX = mousex;
            _lastMouseY = mousey;
        }
    }
}

  
void VRFakeTrackerDeviceRelative::appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents)
{
  while (_pendingEvents.notEmpty()) {
    inputEvents->push(_pendingEvents.getSerializedObject());
    _pendingEvents.pop();
  }
}
  

VRInputDevice*
VRFakeTrackerDeviceRelative::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
    std::string devNameSpace = nameSpace;
  
    std::string trackerName = config->getValue("TrackerName", devNameSpace);
    std::string toggleEvent = config->getValue("ToggleOnOffEvent", devNameSpace);
    std::string rotateEvent = config->getValue("RotateEvent", devNameSpace);
    std::string translateEvent = config->getValue("TranslateEvent", devNameSpace);
    std::string translateZEvent = config->getValue("TranslateZEvent", devNameSpace);
    std::string rollEvent = config->getValue("RollEvent", devNameSpace);
    float xyScale = config->getValueWithDefault("XYTranslationScale", 1.0f, devNameSpace);
    float zScale = config->getValueWithDefault("ZTranslationScale", 1.0f, devNameSpace);
    float rScale = config->getValueWithDefault("RotationScale", 1.0f, devNameSpace);
    int rotationSticky = config->getValueWithDefault("RotationSticky", 0, devNameSpace);
    int rollSticky = config->getValueWithDefault("RollSticky", 0, devNameSpace);
    int translateSticky = config->getValueWithDefault("TranslateSticky", 1, devNameSpace);
    int translateZSticky = config->getValueWithDefault("TranslateZSticky", 1, devNameSpace);
//    VRFloatArray defaultPos {0, 0, -1};
//    VRFloatArray defaultDir {0, 0, 1};
//    VRFloatArray defaultUp  {0, 1, 0};//TODO: Find better constructors that don't require c++ 11
    int pos[] = {0, 0, -1};
    int dir[] = {0, 0, 1};
    int up[] = {0, 1, 0};
    VRFloatArray defaultPos (pos, pos + sizeof(pos) / sizeof(pos[0]));
    VRFloatArray defaultDir (dir, dir + sizeof(dir) / sizeof(dir[0]));
    VRFloatArray defaultUp (up, up + sizeof(up) / sizeof(up[0]));
    VRVector3 startPos = config->getValueWithDefault("StartingPosition", defaultPos, devNameSpace);
    VRVector3 startDir = config->getValueWithDefault("StartingDirection", defaultDir, devNameSpace);
    VRVector3 startUp = config->getValueWithDefault("StartingUp", defaultUp, devNameSpace);
    
    VRFakeTrackerDeviceRelative *dev = new VRFakeTrackerDeviceRelative(trackerName, toggleEvent, rotateEvent, rollEvent, translateEvent, translateZEvent, xyScale, zScale, rScale, rotationSticky, rollSticky, translateSticky, translateZSticky, startPos, startDir, startUp);
    vrMain->addEventHandler(dev);

    return dev;
}

  
} // end namespace


