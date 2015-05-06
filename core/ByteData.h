
#ifndef BYTEDATA_H
#define BYTEDATA_H

#include <cstddef>
#include <string>


/** ByteData
 */
class ByteData {
public:

  // Note: this isn't portable across systems. Not a problem, unless we want to pass byte data over a network or save/load files
  // across different architectures.  For portability, might be better to fix the size for each type at the max for our current 
  // architectures.
  static const int BYTEDATA_SIZEOFINT = sizeof(int);
  static const int BYTEDATA_SIZEOFFLOAT = sizeof(float);
  // TODO: bool
  // TODO: char
  // TODO: double
  // not sure if we need more than that... 


  ByteData() : _byteArray(NULL), _size(0) {}

  ByteData(unsigned char *data, int size) {
    _size = size;
    _byteArray = new unsigned char [_size];
    memcpy(_byteArray, data, _size);
  }

  ByteData(const ByteData &other) {
    _size = other._size;
    if (_size > 0) {
      _byteArray = new unsigned char [_size];
      memcpy(_byteArray, other._byteArray, _size);
    }
    else {
      _byteArray = NULL;
    }
  }

  ByteData& operator=(const ByteData &other) {
    if (this != &other) {
      _size = other._size;
      if (_size > 0) {
	_byteArray = new unsigned char [_size];
	memcpy(_byteArray, other._byteArray, _size);
      }
      else {
	_byteArray = NULL;
      }
    }
    return *this;
  }

  ByteData(int i) {
    _byteArray = new unsigned char [BYTEDATA_SIZEOFINT];
    _size = BYTEDATA_SIZEOFINT;
    packInt(_byteArray, i);
  }

  ByteData(float f) {
    _byteArray = new unsigned char [BYTEDATA_SIZEOFFLOAT];
    _size = BYTEDATA_SIZEOFFLOAT;
    packFloat(_byteArray, f);
  }

  virtual ~ByteData() {
    if (_byteArray) {
      delete [] _byteArray;
    }
  }

  

  /// This exposes the underlying byte array for fast reading of binary data, but be careful, the pointer will
  /// no longer be valid after a call to setData(), an assignment to another ByteData object, or other operation
  /// that modifies the underlying byteArray.
  unsigned char * getByteArray() const { return _byteArray; }
  int getSize() const { return _size; }


  void setData(unsigned char *data, int size) {
    if (_byteArray) {
      delete [] _byteArray;
    }
    _size = size;
    if (_size > 0) {
      _byteArray = new unsigned char [_size];
      memcpy(_byteArray, data, _size);
    }
    else {
      _byteArray = NULL;
    }
  }


  /// Interprets the *entire* bytedata array as holding a single int
  int toInt() const {
    if (_size == BYTEDATA_SIZEOFINT) {
      return unpackInt(_byteArray);
    }
    else {
      // throw an exception
      return 0;
    }
  }

  /// Interprets the *entire* bytedata array as holding a single float
  float toFloat() const {
    if (_size == BYTEDATA_SIZEOFFLOAT) {
      return unpackFloat(_byteArray);
    }
    else {
      // throw an exception
      return 0.0;
    }
  }




  /// Helper Functions -- these assume storage is already allocated, all they do is pack/unpack at a particular location in memory.
  /// TODO: To make these portable for multiple architectures connected over a network, need to check endedness of the system
  /// when packing/unpacking.
  static void packInt(unsigned char *bytePtr, int toPack) {
    unsigned char *p = (unsigned char *) &toPack;
    for (int i=0;i<BYTEDATA_SIZEOFINT;i++) {
      bytePtr[i] = p[BYTEDATA_SIZEOFINT - i - 1];
    }
  }

  static int unpackInt(unsigned char *bytePtr) {
    int toReturn = 0;
    unsigned char *p = (unsigned char *) &toReturn;
    for (int i=0;i<BYTEDATA_SIZEOFINT;i++) {
      p[i] = bytePtr[BYTEDATA_SIZEOFINT - i - 1];
    }
    return toReturn;
  }

  static void packFloat(unsigned char *bytePtr, float toPack) {
    unsigned char *p = (unsigned char *) &toPack;
    for (int i=0;i<BYTEDATA_SIZEOFFLOAT;i++) {
      bytePtr[i] = p[BYTEDATA_SIZEOFFLOAT - i - 1];
    }
  }

  static float unpackFloat(unsigned char *bytePtr) {
    float toReturn = 0;
    unsigned char *p = (unsigned char *) &toReturn;
    for (int i=0;i<BYTEDATA_SIZEOFFLOAT;i++) {
      p[i] = bytePtr[BYTEDATA_SIZEOFFLOAT - i - 1];
    }
    return toReturn;
  }



protected:

  unsigned char *_byteArray;
  int _size;

};


#endif


