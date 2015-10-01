#ifndef VRBYTESTREAM_H
#define VRBYTESTREAM_H

#include <event/VRByteData.h>
#include <iostream>



/** VRByteStream

    This is an I/O stream where the data are contained in an array of bytes.  Since it's a stream, we think of there being
    a current position in that stream.  For example, after reading the first 4 bytes of the data stream and converting them 
    into an int, our position in the stream advances to byte number 5.  
 */
class VRByteStream {
public:

  VRByteStream() {
    _readOffset = 0;
    _writeOffset = 0;
  }

  VRByteStream(int expectedDataSizeInBytes) {
    _readOffset = 0;
    _writeOffset = 0;
    resize(expectedDataSizeInBytes);
  }

  VRByteStream(const VRByteData &data) {
    _byteData = data;
    _readOffset = 0;
    _writeOffset = 0;
  }

 VRByteStream(unsigned char *data, int size) : _byteData(data, size) {
    _readOffset = 0;
    _writeOffset = 0;
  }

  virtual ~VRByteStream() {}


  // Read routines for builtin types

  int readInt() {
    if (_readOffset + VRByteData::BYTEDATA_SIZEOFINT > _byteData.getSize()) {
      // TODO: throw some exception
      std::cerr << "tried to read past the end of a VRByteStream" << std::endl;
      return 0;
    }
    else {
      int i = VRByteData::unpackInt(&_byteData.getByteArray()[_readOffset]);
      _readOffset += VRByteData::BYTEDATA_SIZEOFINT;
      return i;
    }
  }

  float readFloat() {
    if (_readOffset + VRByteData::BYTEDATA_SIZEOFFLOAT > _byteData.getSize()) {
      // TODO: throw some exception
      std::cerr << "tried to read past the end of a VRByteStream" << std::endl;
      return 0;
    }
    else {
      float f = VRByteData::unpackFloat(&_byteData.getByteArray()[_readOffset]);
      _readOffset += VRByteData::BYTEDATA_SIZEOFFLOAT;
      return f;
    }
  }

  std::string readString() {
    int size = readInt();
    if (size <= 0) {
      return std::string();
    }
    else if (_readOffset + size > _byteData.getSize()) {
      // TODO: throw some exception
      std::cerr << "tried to read past the end of a VRByteStream" << std::endl;
      return std::string();      
    }
    else {
      std::string s((const char *)&_byteData.getByteArray()[_readOffset], (size_t)size);
      _readOffset += size;
      return s;
    }    
  }

  VRByteData readByteData() {
    int size = readInt();
    if (size <= 0) {
      return VRByteData();
    }
    else if (_readOffset + size > _byteData.getSize()) {
      // TODO: throw some exception
      std::cerr << "tried to read past the end of a VRByteStream" << std::endl;
      return VRByteData();      
    }
    else {
      VRByteData bd(&_byteData.getByteArray()[_readOffset], size);
      _readOffset += size;
      return bd;
    }
  }
  

  // Write routines for builtin types

  void writeInt(int i) {
    if (_writeOffset + VRByteData::BYTEDATA_SIZEOFINT > _byteData.getSize()) {
      resize(_byteData.getSize() + VRByteData::BYTEDATA_SIZEOFINT);
    }
    VRByteData::packInt(&_byteData.getByteArray()[_writeOffset], i);
    _writeOffset += VRByteData::BYTEDATA_SIZEOFINT;
  }

  void writeFloat(float f) {
    if (_writeOffset + VRByteData::BYTEDATA_SIZEOFFLOAT > _byteData.getSize()) {
      resize(_byteData.getSize() + VRByteData::BYTEDATA_SIZEOFFLOAT);
    }
    VRByteData::packFloat(&_byteData.getByteArray()[_writeOffset], f);
    _writeOffset += VRByteData::BYTEDATA_SIZEOFFLOAT;
  }

  void writeString(const std::string &s) {
    if (_writeOffset + s.size() + VRByteData::BYTEDATA_SIZEOFINT > _byteData.getSize()) {
      resize(_byteData.getSize() + s.size() + VRByteData::BYTEDATA_SIZEOFINT);
    }
    writeInt(s.size());
    memcpy(&_byteData.getByteArray()[_writeOffset], s.c_str(), s.size());
    _writeOffset += s.size();    
  }

  void writeByteData(const VRByteData &data) {
    if (_writeOffset + data.getSize() + VRByteData::BYTEDATA_SIZEOFINT > _byteData.getSize()) {
      resize(_byteData.getSize() + data.getSize() + VRByteData::BYTEDATA_SIZEOFINT);
    }
    writeInt(data.getSize());
    memcpy(&_byteData.getByteArray()[_writeOffset], data.getByteArray(), data.getSize());
    _writeOffset += data.getSize();
  }
  

  int getReadOffset()  { return _readOffset; }
  int getWriteOffset() { return _writeOffset; }

  void setReadOffset(int offset)  { _readOffset = offset; }
  void setWriteOffset(int offset) { _writeOffset = offset; }

 
  // Converts the entire data stream to VRByteData and returns a copy
  VRByteData toByteData() { return _byteData; }

  
  /// This exposes the underlying byte array for fast reading of binary data, but be careful, since the stream can resize
  /// as new bytes are added, this pointer may not be valid after calls that modify the stream.
  unsigned char * getByteArray() const { return _byteData.getByteArray(); }
  int getSize() const { return _byteData.getSize(); }


  void resize(int newSize) {
    if (newSize <= 0) {
      _readOffset = 0;
      _writeOffset = 0;
      _byteData.setData(NULL, 0);
    }
    else if (newSize > _byteData.getSize()) {
      unsigned char *tmp = new unsigned char [newSize];
      memcpy(tmp, _byteData.getByteArray(), _byteData.getSize());
      _byteData.setData(tmp, newSize);
      delete [] tmp;
    }
    else if (newSize < _byteData.getSize()) {
      unsigned char *tmp = new unsigned char [newSize];
      memcpy(tmp, _byteData.getByteArray(), newSize);
      _byteData.setData(tmp, newSize);
      delete [] tmp;
      // Not clear what the best behavior is when the read and write offsets are past the end of the array...
      if (_readOffset >= newSize) {
	_readOffset = newSize;
      }
      if (_writeOffset >= newSize) {
	_writeOffset = newSize;
      }
    }
    // else newSize == _size so nothing to do
  }


protected:
  VRByteData _byteData;
  int _readOffset;
  int _writeOffset;
};

#endif

