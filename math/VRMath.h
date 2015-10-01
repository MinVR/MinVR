#ifndef VRMATH_H
#define VRMATH_H

#include <event/VRByteData.h>
#include <event/VRByteStream.h>
#include <iostream>


/** Vec3
 */
class Vec3 {
public:
  Vec3() : _x(0.0), _y(0.0), _z(0.0) {}

  Vec3(float x, float y, float z) : _x(x), _y(y), _z(z) {}

  Vec3(const VRByteData &data) {
    VRByteStream bs(data);
    deserialize(bs);
  };

  virtual ~Vec3() {}
  
  void serialize(VRByteStream &bstream) {
    bstream.writeFloat(_x);
    bstream.writeFloat(_y);
    bstream.writeFloat(_z);
  }

  void deserialize(VRByteStream &bstream) {
    _x = bstream.readFloat();
    _y = bstream.readFloat();
    _z = bstream.readFloat();
  }

  /*
  void serialize(ByteData &data) {
    unsigned char bytes [3*ByteData::BYTEDATA_SIZEOFFLOAT];
    unsigned char *p = bytes;
    ByteData::packFloat(p, _x);
    p += ByteData::BYTEDATA_SIZEOFFLOAT;
    ByteData::packFloat(p, _y);
    p += ByteData::BYTEDATA_SIZEOFFLOAT;
    ByteData::packFloat(p, _z);
    data.setData(bytes, 3*ByteData::BYTEDATA_SIZEOFFLOAT);
  }

  void deserialize(const ByteData &data) {
    unsigned char * p = data.getByteArray();
    _x = ByteData::unpackFloat(p);
    p += ByteData::BYTEDATA_SIZEOFFLOAT;
    _y = ByteData::unpackFloat(p);
    p += ByteData::BYTEDATA_SIZEOFFLOAT;
    _z = ByteData::unpackFloat(p);
  }
  */

  void print() {
    std::cout << "<" << _x << ", " << _y << ", " << _z << ">" << std::endl;
  }

protected:
  float _x, _y, _z;
};


/** Mat4
    A 4x4 transformation matrix
 */
class Mat4 {
public:
  Mat4() {}
  virtual ~Mat4() {}

};


#endif
