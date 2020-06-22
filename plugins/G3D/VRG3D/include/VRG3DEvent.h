//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

/**
 * \author Daniel Keefe (dfk)
 *
 * \file  Event.h
 * \brief
 *
 */

#ifndef VRG3DEVENT_H
#define VRG3DEVENT_H
#include "CommonInc.H"


namespace MinVR {

class VRG3DEvent;
typedef G3D::ReferenceCountedPointer<class VRG3DEvent> EventRef;

/// Creates a copy of the Event pointed to by e and returns a ref
/// counted pointer to the new copy.
PLUGIN_API EventRef createCopyOfEvent(EventRef e);

/** G3DVR Event class.  To keep things simple, there are no subclasses
    of Event.  The type of data that the event carries is interpreted
    differently based on the value of the type of the event.  Button
    Events are typically sent by devices as two separate
    EVENTTYPE_STANDARD Events, the first named ButtonName_down and
    then when the button is released ButtonName_up.
*/
class VRG3DEvent : public G3D::ReferenceCountedObject
{
public:

  enum EventType {
    EVENTTYPE_STANDARD = 0,        /// standard type, carries no additional info
    EVENTTYPE_1D = 1,              /// event that stores 1D data in a double
    EVENTTYPE_2D = 2,              /// stores two doubles
    EVENTTYPE_3D = 3,              /// stores three doubles
    EVENTTYPE_COORDINATEFRAME = 4, /// stores a G3D::CoordinateFrame
    EVENTTYPE_MSG = 5              /// stores a std::string
  };

  VRG3DEvent(const std::string &name) {
    _name = name;
    _type = EVENTTYPE_STANDARD;
  }

  VRG3DEvent(const std::string &name, const double data) {
    _name = name;
    _data1D = data;
    _type = EVENTTYPE_1D;
  }

  VRG3DEvent(const std::string &name, const G3D::Vector2 &data) {
    _name = name;
    _data2D = data;
    _type = EVENTTYPE_2D;
  }

  VRG3DEvent(const std::string &name, const G3D::Vector3 &data) {
    _name = name;
    _data3D = data;
    _type = EVENTTYPE_3D;
  }

  VRG3DEvent(const std::string &name, const G3D::CoordinateFrame &data) {
    _name = name;
    _dataCF = data;
    _type = EVENTTYPE_COORDINATEFRAME;
  }

  VRG3DEvent(const std::string &name, const std::string &data) {
    _name = name;
    _dataMsg = data;
    _type = EVENTTYPE_MSG;
  }

  virtual ~VRG3DEvent() {}


  std::string     getName() const { return _name; }
  EventType       getType() const { return _type; }

  PLUGIN_API double          get1DData();
  PLUGIN_API G3D::Vector2         get2DData();
  PLUGIN_API G3D::Vector3         get3DData();
  PLUGIN_API G3D::CoordinateFrame getCoordinateFrameData();
  PLUGIN_API std::string     getMsgData();

  PLUGIN_API std::string     toString();

  PLUGIN_API void            rename(const std::string &newname) { _name = newname; }

  PLUGIN_API void serialize(G3D::BinaryOutput &b) const;
  PLUGIN_API void deserialize(G3D::BinaryInput &b);

protected:
  std::string     _name;
  EventType       _type;
  double          _data1D;
  G3D::Vector2         _data2D;
  G3D::Vector3         _data3D;
  G3D::CoordinateFrame _dataCF;
  std::string     _dataMsg;
};


} // end namespace

#endif

