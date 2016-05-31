#include <main/VRFactory.h>

namespace MinVR {

VRDisplayNode* 
VRFactory::createDisplayNode(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) 
{
  for (std::vector<VRDisplayNodeFactory*>::iterator it = _dispNodeFactories.begin(); it < _dispNodeFactories.end(); ++it) {
	  if ((*it)->isType(displaynode, config, nameSpace) || (*it)->isType(window, config, nameSpace)){
      VRDisplayNode *node = (*it)->create(vrMain, config, nameSpace);
      if (node != NULL) {
        node->createChildren(vrMain, config, nameSpace);
        return node;
      }
    }
  }
  return NULL;
}

VRInputDevice* 
VRFactory::createInputDevice(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace)
{
  for (std::vector<VRInputDeviceFactory*>::iterator it = _inputDevFactories.begin(); it < _inputDevFactories.end(); ++it) {
    if((*it)->isType(inputdevice, config, nameSpace)){
      VRInputDevice* dev = (*it)->create(vrMain, config, nameSpace);
      if (dev != NULL) {
        return dev;
      }
    }
  }
  return NULL;
}

VRGraphicsToolkit* 
VRFactory::createGraphicsToolkit(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace)
{
  for (std::vector<VRGraphicsToolkitFactory*>::iterator it = _gfxToolkitFactories.begin(); it < _gfxToolkitFactories.end(); ++it) {
    if((*it)->isType(graphicstoolkit, config, nameSpace)){
      VRGraphicsToolkit* tk = (*it)->create(vrMain, config, nameSpace);
      if (tk != NULL) {
        return tk;
      }
    }
  }
  return NULL;
}


VRWindowToolkit* 
VRFactory::createWindowToolkit(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace)
{
  for (std::vector<VRWindowToolkitFactory*>::iterator it = _winToolkitFactories.begin(); it < _winToolkitFactories.end(); ++it) {
    if((*it)->isType(windowtoolkit, config, nameSpace)){
      VRWindowToolkit* tk = (*it)->create(vrMain, config, nameSpace);
      if (tk != NULL) {
        return tk;
      }
    }
  }
  return NULL;
}


} // end namespace
