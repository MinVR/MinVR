#include <main/VRFactory.h>
#include <main/VRMainInterface.h>
#include <typeinfo>

namespace MinVR {

void VRFactory::addSubFactory(VRItemFactory* factory) {
	_itemFactories.push_back(factory);
}

} // end namespace
