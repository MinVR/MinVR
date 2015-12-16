#include "VREvent.h"

VREvent::VREvent(const std::string &eventName,
                 const VRDataIndex &dataIndex) :
  _name(eventName), _dataIndex(dataIndex) {}

VREvent::VREvent() {}

// VRDatum objects inside VRDataIndex are reference counted, so they
// will clean themselves up.
VREvent::~VREvent() {}

std::string VREvent::toXML() {
  return _dataIndex.serialize(_name);
}


VREvent VREvent::fromXML(const std::string &xml, std::string &leftoverInput) {
  

  std::map<std::string, std::string> props;
  std::string dataIndexXML;
  XMLUtils::getXMLField(xml, "VREvent", props, dataIndexXML, leftoverInput);
  
  return VREvent(props["name"], VRDataIndex::fromXML(dataIndexXML));
}

VREvent VREvent::fromXML(const std::string &xml) {
  std::string leftoverInput;
  return fromXML(xml, leftoverInput);
}
