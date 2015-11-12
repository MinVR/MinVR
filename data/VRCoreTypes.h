#ifndef VRCORETYPES_H
#define VRCORETYPES_H

#include <vector>
#include <list>

// VRCoreTypes
typedef int                      VRInt;
typedef double                   VRDouble;
typedef std::string              VRString;
typedef std::vector<int>         VRIntArray;
typedef std::vector<double>      VRDoubleArray;
typedef std::vector<std::string> VRStringArray;

// Tom: VRContainer seems a bit different than the rest, does it make sense to consider
// it one of the core types defined in the enum above and here?
typedef std::list<std::string>   VRContainer;


/// An ID for each of the VRCoreTypes.  This is step 1
/// in the specialization instructions below.
typedef enum
{
  VRCORETYPE_NONE        = 0,
  VRCORETYPE_INT         = 1,
  VRCORETYPE_DOUBLE      = 2,
  VRCORETYPE_STRING      = 3,
  VRCORETYPE_INTARRAY    = 4,
  VRCORETYPE_DOUBLEARRAY = 5,
  VRCORETYPE_STRINGARRAY = 6,
  VRCORETYPE_CONTAINER   = 7
} VRCORETYPE_ID;

#define VRCORETYPE_NTYPES 8




// ***** TO HELP WITH CONVERSTIONS FROM OTHER TYPES TO THE VR CORE TYPES *****


/** This class provides an interface for classes that wish to be convertable to
 a VRInt.  If this is defined, then the class can be added to a VRDataIndex
 with a simple call to dataIndex->addData("MyClass", myclass);
 */
class VRIntConvertable {
public:
  virtual VRInt toVRInt() = 0;
};

/** This class provides an interface for classes that wish to be convertable to
 a VRDouble.  If this is defined, then the class can be added to a VRDataIndex
 with a simple call to dataIndex->addData("MyClass", myclass);
 */
class VRDoubleConvertable {
public:
  virtual VRDouble toVRDouble() = 0;
};

/** This class provides an interface for classes that wish to be convertable to
 a VRString.  If this is defined, then the class can be added to a VRDataIndex
 with a simple call to dataIndex->addData("MyClass", myclass);
 */
class VRStringConvertable {
public:
  virtual VRString toVRString() = 0;
};

/** This class provides an interface for classes that wish to be convertable to
 a VRIntArray.  If this is defined, then the class can be added to a VRDataIndex
 with a simple call to dataIndex->addData("MyClass", myclass);
 */
class VRIntArrayConvertable {
public:
  virtual VRIntArray toVRIntArray() = 0;
};

/** This class provides an interface for classes that wish to be convertable to
 a VRDoubleArray.  If this is defined, then the class can be added to a VRDataIndex
 with a simple call to dataIndex->addData("MyClass", myclass);
 */
class VRDoubleArrayConvertable {
public:
  virtual VRDoubleArray toVRDoubleArray() = 0;
};

/** This class provides an interface for classes that wish to be convertable to
 a VRStringArray.  If this is defined, then the class can be added to a VRDataIndex
 with a simple call to dataIndex->addData("MyClass", myclass);
 */
class VRStringArrayConvertable {
public:
  virtual VRStringArray toVRStringArray() = 0;
};




// ***** TO HELP WITH CONVERSTIONS FROM THE VR CORE TYPES TO OTHER TYPES *****




#endif
