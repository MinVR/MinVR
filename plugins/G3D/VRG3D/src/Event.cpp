//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


#include "../include/Event.h"
#include "../include/G3DOperators.h"

namespace MinVR {

using namespace G3D;

double
Event::get1DData()
{
  alwaysAssertM(_type==EVENTTYPE_1D, "Trying to access 1D data from a non-1D event.");
  return _data1D;
}

Vector2
Event::get2DData()
{
  alwaysAssertM(_type==EVENTTYPE_2D, "Trying to access 2D data from a non-2D event.");
  return _data2D;
}

Vector3
Event::get3DData()
{
  alwaysAssertM(_type==EVENTTYPE_3D, "Trying to access 3D data from a non-3D event.");
  return _data3D;
}

CoordinateFrame
Event::getCoordinateFrameData()
{
  alwaysAssertM(_type==EVENTTYPE_COORDINATEFRAME, "Trying to access coordinateframe data from a non-cf event.");
  return _dataCF;
}

std::string
Event::getMsgData()
{
  alwaysAssertM(_type==EVENTTYPE_MSG, "Trying to access message data from a non-msg event.");
  return _dataMsg;
}

std::string
Event::toString()
{
  switch (_type) {
  case EVENTTYPE_1D:
    return format("%s (%.2f)", _name.c_str(), _data1D);
    break;
  case EVENTTYPE_2D:
    return format("%s (%.2f,%.2f)", _name.c_str(), _data2D[0], _data2D[1]);
    break;
  case EVENTTYPE_3D:
    return format("%s (%.2f,%.2f,%.2f)", _name.c_str(), _data3D[0], _data3D[1], _data3D[2]);
    break;
  case EVENTTYPE_COORDINATEFRAME:
    return _name + " " + matrix4ToString(_dataCF.toMatrix4());
    break;
  case EVENTTYPE_MSG:
    return _name + " " + _dataMsg;
    break;
  default:
    return _name;
    break;
  }
}


void
Event::serialize(BinaryOutput &b) const
{
  b.writeInt32(_name.size());
  b.writeString(_name);
  b.writeInt8((int8)_type);
  switch (_type) {
  case EVENTTYPE_STANDARD:
    break;
  case EVENTTYPE_1D: {
    float64 d = _data1D;
    b.writeFloat64(d);
    break;
  }
  case EVENTTYPE_2D:
    b.writeVector2(_data2D);
    break;
  case EVENTTYPE_3D:
    b.writeVector3(_data3D);
    break;
  case EVENTTYPE_COORDINATEFRAME:
    _dataCF.serialize(b);
    break;
  case EVENTTYPE_MSG:
    b.writeInt32(_dataMsg.size());
    b.writeString(_dataMsg);
    break;
  }
}

void
Event::deserialize(BinaryInput &b)
{
  int size = b.readInt32();
  _name    = b.readString(size+1);
  _type    = (EventType)b.readInt8();
  switch (_type) {
  case EVENTTYPE_STANDARD:
    break;
  case EVENTTYPE_1D:
    _data1D  = b.readFloat64();
    break;
  case EVENTTYPE_2D:
    _data2D  = b.readVector2();
    break;
  case EVENTTYPE_3D:
    _data3D  = b.readVector3();
    break;
  case EVENTTYPE_COORDINATEFRAME:
    _dataCF.deserialize(b);
    _dataCF.rotation.orthonormalize();
    break;
  case EVENTTYPE_MSG: {
    int size = b.readInt32();
    _dataMsg = b.readString(size+1);
    break;
  }
  default:
    // I got to this with windows server/client at Duke's Cave!!
    alwaysAssertM(false, "Error, unknown eventtype in deserialize method.");
    break;
  }
}



EventRef
createCopyOfEvent(EventRef e) {
  switch (e->getType()) {
  case Event::EVENTTYPE_STANDARD:
    return new Event(e->getName());
    break;
  case Event::EVENTTYPE_1D:
    return new Event(e->getName(),e->get1DData());
    break;
  case Event::EVENTTYPE_2D:
    return new Event(e->getName(),e->get2DData());
    break;
  case Event::EVENTTYPE_3D:
    return new Event(e->getName(),e->get3DData());
    break;
  case Event::EVENTTYPE_COORDINATEFRAME:
    return new Event(e->getName(),e->getCoordinateFrameData());
    break;
  case Event::EVENTTYPE_MSG:
    return new Event(e->getName(),e->getMsgData());
    break;
  default:
    alwaysAssertM(false, "createCopyOfEvent: Unknown event type!");
    return NULL;
    break;
  }
}


} // end namespace


