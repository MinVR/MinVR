
#ifndef VREVENT_H
#define VREVENT_H

#include <string>
#include <event/VRByteData.h>
#include <event/VRByteStream.h>


/** VREvent
 */
class VREvent {
public:

  VREvent(const std::string &name, const VRByteData &data) : _name(name), _data(data) {}

  template<typename SERIALIZABLE_TYPE>
  VREvent(const std::string &name, SERIALIZABLE_TYPE &object) : _name(name) {
    VRByteStream bs;
    object.serialize(bs);
    _data = bs.toByteData();
  }

  VREvent() {}

  virtual ~VREvent() {}

  std::string getName() const { return _name; }

  /// For speed (to avoid copying the data) returns a const reference to the data. 
  /// Make sure to only use this reference while the VREvent is within scope.
  const VRByteData& getData() const { return _data; }

  void serialize(VRByteStream &bs) const {
    bs.writeString(_name);
    bs.writeByteData(_data);
  }

  void deserialize(VRByteStream &bs) {
    _name = bs.readString();
    _data = bs.readByteData();
  }

  /// If the way that VREvent's are serialized is changed, this function must be updated as well.  This is used
  /// by VRNetInterface to calculate the size of buffer needed to serialize an array of VREvents to send them
  /// over the network.
  int getSizeInBytes() const {
    // dataSize = (bytes needed to store InputEvent's name string) + (bytes needed to store the InputEvent's data)
    return (_name.length() + VRByteData::BYTEDATA_SIZEOFINT) + (_data.getSize() + VRByteData::BYTEDATA_SIZEOFINT);
  }


protected:
  std::string _name;
  VRByteData _data;
};


#endif
