/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRITEMFACTORY_H_
#define VRITEMFACTORY_H_

#include <string>
#include <main/VRMainInterface.h>
#include <display/VRDisplayNode.h>
#include <typeinfo>

namespace MinVR {

template <typename T>
class VRSpecificItemFactory;

/** VRItemFactory is an abstraction subclass which all other factories inherit from.  The class exists as a base class which
 * can be stored in an array in the VRFactory class.  The createItem method is templated so it allows for generic implementations
 * for different item types (i.e. VRDisplayNodes, VRInputDevices, VRToolkits, custom plugin objects, etc...)
*/
class VRItemFactory {
public:
	virtual ~VRItemFactory() {}

	/// Generic create item method which determines the type of the factory and uses it to create items if it is the correct type.
	template <typename T>
	T* createItem(VRMainInterface *vrMain, VRDataIndex *config, const std::string &dataContainer) {

    if (getType() == typeid(T)) {

      VRSpecificItemFactory<T>* factory = static_cast<VRSpecificItemFactory<T>*>(this);
			if(factory) {

				return factory->create(vrMain, config, dataContainer);
			}
		}

		return NULL;
	}

	/// virtual abstract method which gets the type from the derived class.
	virtual const std::type_info& getType() = 0;
};

/** VRSpecificItemFactory is a specific type of class which allows subclasses to inherit the basic
 * factory implementation.  For example, it first checks that they typeName equals the same type in the
 * VRDataIndex and passes the correct namespace to the underlying concrete implementation.  If one wants
 * to create a factory for a type, this factory should be inherited from.
*/
template <typename T>
class VRSpecificItemFactory : public VRItemFactory {
public:
	/// Allows subclasses to specify the type name in the VRDataIndex
	VRSpecificItemFactory(const std::string& typeName) : _typeName(typeName) {
  }
	virtual ~VRSpecificItemFactory() {}

	/// First checks that the typeName maches the VRDataIndex and then passes the type to the concrete implementation
	virtual T* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &dataContainer) {
		std::string attributeName = T::getAttributeName();

		std::string type = config->getAttributeValue(dataContainer, attributeName);

		if (type != _typeName) {
			// This factory cannot create the type specified
			return NULL;
		}

		return createConcrete(vrMain, config, dataContainer);
	}

protected:
	/// Concrete implementation to be defined in the subclasses
	virtual T* createConcrete(VRMainInterface *vrMain, VRDataIndex *config, const std::string &itemNameSpace) = 0;

	/// Returns the specific type id used in the main VRItemFactory
	const std::type_info& getType() {
		return typeid(T);
	}

private:
	std::string _typeName;
};

/** VRConcreteItemFactory allows users to specify a static create(...) method instead of creating a factory for the item.
 * The ParentType is the generic type that is requested from the ItemFactory (i.e. VRDisplayNode, etc...).
 * It will not compile if T is not derived from ParentType.
*/
template <typename ParentType, typename T>
class VRConcreteItemFactory : public VRSpecificItemFactory<ParentType> {
public:
	VRConcreteItemFactory(const std::string& typeName) : VRSpecificItemFactory<ParentType>(typeName) {}
	virtual ~VRConcreteItemFactory() {}

	/// Calls static create(...) method specified by the type.
	ParentType* createConcrete(VRMainInterface *vrMain, VRDataIndex *config, const std::string &itemNameSpace) {
		ParentType* item = T::create(vrMain, config, itemNameSpace);
		if (typeid(ParentType) == typeid(VRDisplayNode))
		{
			VRDisplayNode* displayNode = dynamic_cast<VRDisplayNode*>(item);
			displayNode->createChildren(vrMain, config, itemNameSpace);
		}
		return item;
	}
};


} /* namespace MinVR */

#endif /* VRITEMFACTORY_H_ */
