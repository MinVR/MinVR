#include "VREvent.h"
#include <data/XMLUtils.h>

VREvent::VREvent(const std::string &eventName, const VRDataIndex &dataIndex) : _name(eventName), _dataIndex(dataIndex) {
}

VREvent::VREvent() {
}

// AnyCoreType objects inside VRDataIndex are reference counted, so they will clean up themselves
VREvent::~VREvent() {
}


std::string VREvent::toXML() {
  std::map<std::string, std::string> props;
  props["name"] = _name;
  return XMLUtils::writeXMLField("VREvent", props, _dataIndex.toXML());
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