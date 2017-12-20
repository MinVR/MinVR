#ifndef VRFACTORY_H
#define VRFACTORY_H

#include <display/VRDisplayNode.h>
#include <display/VRGraphicsToolkit.h>
#include <display/VRWindowToolkit.h>
#include <input/VRInputDevice.h>
#include <main/VRLog.h>
#include <main/VRMainInterface.h>
#include <main/VRItemFactory.h>

namespace MinVR {

class VRMain;

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

  /// Creates a new item from registered sub-factories.
  template <typename T>
  T* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &dataContainer);

  /// Plugins call this method to register an item type which has a static create
  /// method with the same signature as the VRItemFactory create method.
  template <typename ParentType, typename T>
  void registerItemType(const std::string typeName);

  /// Plugins call this method to add a new "sub-factory" to this master factory.
  void addSubFactory(VRItemFactory* factory);

  std::vector<std::string> getRegisteredTypes() { return _registeredTypes; };

protected:
  std::vector<VRItemFactory*> _itemFactories;

  // It's useful to keep around a list of what we do know about so we can be
  // more informative when someone asks for something we *don't* know about.
  std::vector<std::string> _registeredTypes;
};

template <typename T>
T* VRFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &dataContainer) {

  // Run through all the items in the factory trying to generate something of the
  // correct type.
  for (std::vector<VRItemFactory*>::iterator it = _itemFactories.begin();
       it < _itemFactories.end(); ++it) {

    T* item = (*it)->createItem<T>(vrMain, config, dataContainer);
    if (item != NULL) {
      // Success; we have created something.  Return it.
      return item;
    }
  }

  // Issue a warning here, but we assume the caller will issue an error when it
  // gets a NULL back.  We let the caller kill off the process because it has more
  // information with which to make a decent error message.
  std::string typeStr = std::string(typeid(T).name());
  std::string helpmsg = std::string("This might be caused by a typo in your config file where the subtype (inputdeviceType, displaynodeType, etc) is specified.\n") + 
    "Or if " + typeStr + " is provided by a plugin, there might have been a problem loading that plugin.\n" +
    "VRFactory is currently aware of the following types:";
  for (std::vector<std::string>::iterator it = _registeredTypes.begin(); it < _registeredTypes.end(); ++it) {
    helpmsg += "  " + *it + "\n";
  } 
  VRWARNING("VRFactory was unable to create Nothing in the factory catalog with the type (" +
          typeStr + ") and subtype you specified.", helpmsg);

  // Return NULL to indicate failure.
  return NULL;
}

template <typename ParentType, typename T>
void VRFactory::registerItemType(const std::string typeName) {

  _registeredTypes.push_back(typeName);

  addSubFactory(new VRConcreteItemFactory<ParentType, T>(typeName));
}

} // end namespace

#endif
