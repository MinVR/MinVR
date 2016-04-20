#include <main/VRFactory.h>

namespace MinVR {

VRDisplayNode* 
VRFactory::createDisplayNode(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) 
{
  for (std::vector<VRDisplayNodeFactory*>::iterator it = _dispNodeFactories.begin(); it < _dispNodeFactories.end(); ++it) {
    VRDisplayNode *node = (*it)->create(vrMain, config, valName, nameSpace);
    if (node != NULL) {
      return node;
    }
  }
  return NULL;
}

VRInputDevice* 
VRFactory::createInputDevice(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace)
{
  for (std::vector<VRInputDeviceFactory*>::iterator it = _inputDevFactories.begin(); it < _inputDevFactories.end(); ++it) {
    VRInputDevice* dev = (*it)->create(vrMain, config, valName, nameSpace);
    if (dev != NULL) {
      return dev;
    }
  }
  return NULL;
}

VRGraphicsToolkit* 
VRFactory::createGraphicsToolkit(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace)
{
  for (std::vector<VRGraphicsToolkitFactory*>::iterator it = _gfxToolkitFactories.begin(); it < _gfxToolkitFactories.end(); ++it) {
    VRGraphicsToolkit* tk = (*it)->create(vrMain, config, valName, nameSpace);
    if (tk != NULL) {
      return tk;
    }
  }
  return NULL;
}


VRWindowToolkit* 
VRFactory::createWindowToolkit(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace)
{
  for (std::vector<VRWindowToolkitFactory*>::iterator it = _winToolkitFactories.begin(); it < _winToolkitFactories.end(); ++it) {
    VRWindowToolkit* tk = (*it)->create(vrMain, config, valName, nameSpace);
    if (tk != NULL) {
      return tk;
    }
  }
  return NULL;
}


} // end namespace
