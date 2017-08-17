#ifndef VRFACTORY_H
#define VRFACTORY_H

#include <display/VRDisplayNode.h>
#include <display/VRGraphicsToolkit.h>
#include <display/VRWindowToolkit.h>
#include <input/VRInputDevice.h>
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

  /// Plugins call this method to register an item type which has a static create method with the same signature
  /// as an the VRItemFactory create method.
  template <typename ParentType, typename T>
  void registerItemType(const std::string typeName);

  /// Plugins call this mehod to add a new "sub-factory" to this master factory
  void addSubFactory(VRItemFactory* factory);

protected:
  std::vector<VRItemFactory*> _itemFactories;
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
  VRWARNING("Nothing in the factory catalog with the correct type: " +
            std::string(typeid(T).name()),
            "This is usually caused by a typo in your config file.");
  return NULL;
}

template <typename ParentType, typename T>
void VRFactory::registerItemType(const std::string typeName) {

	  addSubFactory(new VRConcreteItemFactory<ParentType, T>(typeName));
}

} // end namespace

#endif
