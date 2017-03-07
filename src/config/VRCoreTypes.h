// -*-c++-*-
#ifndef VRCORETYPES_H
#define VRCORETYPES_H

#include <vector>
#include <list>
#include <string>

namespace MinVR {

/// This is the list of data types we can handle.  This is step 1
/// in the specialization instructions below.

// VRCoreTypes
typedef int                      VRInt;
typedef float                    VRFloat;
typedef std::string              VRString;
typedef std::vector<int>         VRIntArray;
typedef std::vector<float>       VRFloatArray;
typedef std::vector<std::string> VRStringArray;

// This one is a bit different than the others, but is still a core type.
typedef std::list<std::string>   VRContainer;

// typedef int MVRInt;
// typedef float MVRFloat;
// typedef std::string MVRString;
// // An MVRContainer is actually a list of strings.
// typedef std::list<std::string> MVRContainer;
// typedef std::vector<float> MVRArrayFloat;


typedef enum
{
  VRCORETYPE_NONE        = 0,
  VRCORETYPE_INT         = 1,
  VRCORETYPE_FLOAT       = 2,
  VRCORETYPE_STRING      = 3,
  VRCORETYPE_INTARRAY    = 4,
  VRCORETYPE_FLOATARRAY  = 5,
  VRCORETYPE_STRINGARRAY = 6,
  VRCORETYPE_CONTAINER   = 7
} VRCORETYPE_ID;

#define VRCORETYPE_NTYPES 8

// The classes below are here to help with conversions from other
// types to the MinVR2 core types.  If some class inherits from one of
// these convertible classes, and has the conversion method
// implemented, then those objects can be added to a VRDataIndex by
// calling dataIndex->addData("MyClass", myclass).


// Convert to a VRInt
class VRIntConvertible {
public:
  virtual VRInt toVRInt() const = 0;
};

// Convert to a VRFloat
class VRFloatConvertible {
public:
  virtual VRFloat toVRFloat() const = 0;
};

// Convert to a VRString
class VRStringConvertible {
public:
  virtual VRString toVRString() const = 0;
};

// Convert to a VRIntArray
class VRIntArrayConvertible {
public:
  virtual VRIntArray toVRIntArray() const = 0;
};

// Convert to a VRFloatArray
class VRFloatArrayConvertible {
public:
  virtual VRFloatArray toVRFloatArray() const = 0;
};

// Convert to a VRStringArray
class VRStringArrayConvertible {
public:
  virtual VRStringArray toVRStringArray() const = 0;
};

// Convert to a VRContainer
class VRContainerConvertible {
public:
  virtual VRContainer toVRContainer() const = 0;
};

} // end namespace MinVR
#endif
