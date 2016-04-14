#include "VRFactory.h"

namespace MinVR {

VRDisplayNode* 
VRFactory::createDisplayNode(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) 
{
  for (_dispDevFactories::iterator it = _dispDevFactories.begin(); it < _dispDevFactories.end(); ++it) {
    VRDisplayDevicee* dev = it->create(vrMain, config, valName, nameSpace);
    if (dev != NULL) {
      return dev;
    }
  }
  return NULL;
}

VRInputDevice* 
VRFactory::createInputDevice(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace)
{
  for (_inputDevFactories::iterator it = _inputDevFactories.begin(); it < _inputDevFactories.end(); ++it) {
    VRInputDevicee* dev = it->create(vrMain, config, valName, nameSpace);
    if (dev != NULL) {
      return dev;
    }
  }
  return NULL;
}

VRGraphicsToolkit* 
VRFactory::createGraphicsToolkit(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace)
{
  for (_gfxToolkitFactories::iterator it = _gfxToolkitFactories.begin(); it < _gfxToolkitFactories.end(); ++it) {
    VRGraphicsToolkit* tk = it->create(vrMain, config, valName, nameSpace);
    if (tk != NULL) {
      return tk;
    }
  }
  return NULL;
}


VRWindowToolkit* 
VRFactory::createWindowToolkit(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace)
{
  for (_winToolkitFactories::iterator it = _winToolkitFactories.begin(); it < _winToolkitFactories.end(); ++it) {
    VRWindowToolkit* tk = it->create(vrMain, config, valName, nameSpace);
    if (tk != NULL) {
      return tk;
    }
  }
  return NULL;
}


} // end namespace
