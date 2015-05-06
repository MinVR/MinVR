#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include "ByteData.H"
#include <iostream>



/** ByteStream

    This is an I/O stream where the data are contained in an array of bytes.  Since it's a stream, we think of there being
    a current position in that stream.  For example, after reading the first 4 bytes of the data stream and converting them 
    into an int, our position in the stream advances to byte number 5.  
 */
class ByteStream {
public:

  ByteStream() {
    _readOffset = 0;
    _writeOffset = 0;
  }

  ByteStream(int expectedDataSizeInBytes) {
    _readOffset = 0;
    _writeOffset = 0;
    resize(expectedDataSizeInBytes);
  }

  ByteStream(const ByteData &data) {
    _byteData = data;
    _readOffset = 0;
    _writeOffset = 0;
  }

 ByteStream(unsigned char *data, int size) : _byteData(data, size) {
    _readOffset = 0;
    _writeOffset = 0;
  }

  virtual ~ByteStream() {}


  // Read routines for builtin types

  int readInt() {
    if (_readOffset + ByteData::BYTEDATA_SIZEOFINT > _byteData.getSize()) {
      // TODO: throw some exception
      std::cerr << "tried to read past the end of a ByteStream" << std::endl;
      return 0;
    }
    else {
      int i = ByteData::unpackInt(&_byteData.getByteArray()[_readOffset]);
      _readOffset += ByteData::BYTEDATA_SIZEOFINT;
      return i;
    }
  }

  float readFloat() {
    if (_readOffset + ByteData::BYTEDATA_SIZEOFFLOAT > _byteData.getSize()) {
      // TODO: throw some exception
      std::cerr << "tried to read past the end of a ByteStream" << std::endl;
      return 0;
    }
    else {
      float f = ByteData::unpackFloat(&_byteData.getByteArray()[_readOffset]);
      _readOffset += ByteData::BYTEDATA_SIZEOFFLOAT;
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
      std::cerr << "tried to read past the end of a ByteStream" << std::endl;
      return std::string();      
    }
    else {
      std::string s((const char *)&_byteData.getByteArray()[_readOffset], (size_t)size);
      _readOffset += size;
      return s;
    }    
  }

  ByteData readByteData() {
    int size = readInt();
    if (size <= 0) {
      return ByteData();
    }
    else if (_readOffset + size > _byteData.getSize()) {
      // TODO: throw some exception
      std::cerr << "tried to read past the end of a ByteStream" << std::endl;
      return ByteData();      
    }
    else {
      ByteData bd(&_byteData.getByteArray()[_readOffset], size);
      _readOffset += size;
      return bd;
    }
  }
  

  // Write routines for builtin types

  void writeInt(int i) {
    if (_writeOffset + ByteData::BYTEDATA_SIZEOFINT > _byteData.getSize()) {
      resize(_byteData.getSize() + ByteData::BYTEDATA_SIZEOFINT);
    }
    ByteData::packInt(&_byteData.getByteArray()[_writeOffset], i);
    _writeOffset += ByteData::BYTEDATA_SIZEOFINT;
  }

  void writeFloat(float f) {
    if (_writeOffset + ByteData::BYTEDATA_SIZEOFFLOAT > _byteData.getSize()) {
      resize(_byteData.getSize() + ByteData::BYTEDATA_SIZEOFFLOAT);
    }
    ByteData::packFloat(&_byteData.getByteArray()[_writeOffset], f);
    _writeOffset += ByteData::BYTEDATA_SIZEOFFLOAT;
  }

  void writeString(const std::string &s) {
    if (_writeOffset + s.size() + ByteData::BYTEDATA_SIZEOFINT > _byteData.getSize()) {
      resize(_byteData.getSize() + s.size() + ByteData::BYTEDATA_SIZEOFINT);
    }
    writeInt(s.size());
    memcpy(&_byteData.getByteArray()[_writeOffset], s.c_str(), s.size());
    _writeOffset += s.size();    
  }

  void writeByteData(const ByteData &data) {
    if (_writeOffset + data.getSize() + ByteData::BYTEDATA_SIZEOFINT > _byteData.getSize()) {
      resize(_byteData.getSize() + data.getSize() + ByteData::BYTEDATA_SIZEOFINT);
    }
    writeInt(data.getSize());
    memcpy(&_byteData.getByteArray()[_writeOffset], data.getByteArray(), data.getSize());
    _writeOffset += data.getSize();
  }
  

  int getReadOffset()  { return _readOffset; }
  int getWriteOffset() { return _writeOffset; }

  void setReadOffset(int offset)  { _readOffset = offset; }
  void setWriteOffset(int offset) { _writeOffset = offset; }

 
  // Converts the entire data stream to ByteData and returns a copy
  ByteData toByteData() { return _byteData; }

  
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
  ByteData _byteData;
  int _readOffset;
  int _writeOffset;
};

#endif

