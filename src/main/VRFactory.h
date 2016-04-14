#ifndef VRFACTORY_H
#define VRFACTORY_H

namespace MinVR {

#include "display/VRDisplayNode.h"
#include "display/VRGraphicsToolkit.h"
#include "display/VRWindowToolkit.h"
#include "input/VRInputDevice.h"
#include "main/VRMainInterface.h"


class VRMain;


/** Abstract base class for display node factories, which get added as "sub-factories" to the main 
    VRFactory class.  This sub-factory strategy is used as part of the plugin strategy: 1. plugins
    define objects, like a specific VRDisplayNode and a specific VRDisplayNodeFactory to create
    them.  2. When the plugins are loaded, they register their factory methods with the master 
    VRFactory as "sub-factories".  3. VRMain (or other parts of MinVR) use VRFactory to create objects
    defined in config files by calling VRFactory methods. 
 */
class VRDisplayNodeFactory {
public:
  virtual ~VRDisplayNodeFactory() {}

  VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) = 0;
};


/** See description for VRDisplayDeviceFactory... same idea here but for input devices.
*/
class VRInputDeviceFactory {
public:
  virtual ~VRInputDeviceFactory() {}

  VRInputDevice* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) = 0;
};

/**
 */
class VRGraphicsToolkitFactory {
public:
  virtual ~VRGraphicsToolkitFactory() {}

  VRGraphicsToolkit* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) = 0;
};


/**
 */
class VRWindowToolkitFactory {
public:
  virtual ~VRWindowToolkitFactory() {}

  VRWindowToolkit* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) = 0;
};


/** The master factory class for MinVR.  This factory knows how to create a variety of new objects 
    from config file settings loaded into a VRDataIndex.  Since many of the objects we want to create
    are defined in plugins, we cannot include all of the logic to create them here.  So, instead small
    factories are created inside the plugins, and then when each plugin is loaded it registers its 
    factories with this master VRFactory.  VRMain and other parts of the core MinVR code can then
    use VRFactory to create objects from config settings even if those objects are defined in plugins.
*/
class VRFactory {
public:
  virtual ~VRFactory() {}

  /// Creates a new display node and children from config data using the VRDisplayNode sub-factories as needed.
  VRDisplayNode* createDisplayNode(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace);

  /// Creates a new input device from config data using the VRInputDevice sub-factories as needed.
  VRInputDevice* createInputDevice(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace);

  /// Creates a new graphics toolkit from sub-factories.
  VRGraphicsToolkit* createGraphicsToolkit(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace);

  /// Creates a new window toolkit from sub-factories.
  VRWindowToolkit* createWindowToolkit(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace);


  /// Plugins call this mehod to add a new "sub-factory" to this master factory
  void addSubFactory(VRDisplayNodeFactory *factory) {
    _dispNodeFactories.push_back(factory);
  }

  void addSubFactory(VRInputDeviceFactory *factory) {
    _inputDevFactories.push_back(factory);
  }

  void addSubFactory(VRGraphicsToolkitFactory *factory) {
    _gfxToolkitFactories.push_back(factory);
  }

  void addSubFactory(VRWindowToolkitFactory *factory) {
    _winToolkitFactories.push_back(factory);
  }

protected:
  std::vector<VRDisplayNodeFactory*> _dispNodeFactories;
  std::vector<VRInputDeviceFactory*> _inputDevFactories;
  std::vector<VRGraphicsToolkitFactory*> _gfxToolkitFactories;
  std::vector<VRWindowToolkitFactory*> _winToolkitFactories;

};


} // end namespace

#endif
