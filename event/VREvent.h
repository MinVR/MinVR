
#ifndef VREVENT_H
#define VREVENT_H

#include <cstring>
#include <string>
#include <config/VRDataIndex.h>


/** VREvent
 */
class VREvent {
public:

  VREvent(const std::string &eventName, const VRDataIndex &dataIndex);
  /// Creates an empty event
  VREvent();

  virtual ~VREvent();

  /// Returns the name of the VREvent, not be to confused with the
  /// name(s) of specific VRDatum objects stored inside _dataIndex.
  /// For example, an event named "Mouse_Move" can have an element
  /// named "Postion" and another named "Velocity", etc.  This method
  /// returns the event name (e.g., "Mouse_Move").  Use
  /// getDataIndex->getNames() to get "Position" and "Velocity", etc.
  std::string getName() const { return _name; }
  void setName(const std::string &name) { _name = name; }

  /// Access event data through the dataIndex object
  VRDataIndex* getDataIndex() { return &_dataIndex; }

  /// Serialize the event in an XML format
  std::string toXML();

  /// A static constructor that creates a new VREvent from XML format data
  static VREvent fromXML(const std::string &xml);
  /// A static constructor that creates a new VREvent from XML format data
  static VREvent fromXML(const std::string &xml, std::string &leftoverInput);
  
protected:
  std::string _name;
  VRDataIndex _dataIndex;
};


#endif
