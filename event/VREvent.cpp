#include "VREvent.h"

VREvent::VREvent(const std::string &eventName,
                 const VRDataIndex &dataIndex) :
  _name(eventName), _dataIndex(dataIndex) {}

VREvent::VREvent(const std::string &serialized_string)
{
	_name = _dataIndex.addSerializedValue(serialized_string);
}

VREvent::VREvent()
{
}

// VRDatum objects inside VRDataIndex are reference counted, so they
// will clean themselves up.
VREvent::~VREvent()
{
}

std::string VREvent::toXML() {
  return _dataIndex.serialize(_name);
}

