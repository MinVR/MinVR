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
  return writeXMLField("VREvent", props, _dataIndex.toXML());
}


VREvent VREvent::fromXML(const std::string &xml) {
  std::map<std::string, std::string> props;
  std::string dataIndexXML;
  std::string leftover;
  getXMLField(xml, "VREvent", props, dataIndexXML, leftover);
  
  return VREvent(props["name"], VRDataIndex::fromXML(dataIndexXML));
}

