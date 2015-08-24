// -*-c++-*-
#ifndef MINVR_DATUM_H
#define MINVR_DATUM_H

#include <map>
#include <list>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <sstream>

// This is used to separate values in a serialized vector.
// use static_case<char>MINVRSEPARATOR where a char is needed.
#define MINVRSEPARATOR "^"

/// This is the list of data types we can handle.  This is step 4
/// in the specialization instructions below.
typedef enum
{
  NONE          = 0,
  MVRINT        = 1,
  MVRFLOAT      = 2,
  MVRSTRING     = 3,
  MVRVEC_INT    = 4,
  MVRVEC_FLOAT  = 5,
  MVRVEC_STRING = 6,
  MVRCONTAINER  = 7
} MVRTYPE_ID;


// This class is meant to hold a data object of some arbitrary type
// (ok, arbitrary within a small range of types), and to serialize and
// deserialize it into a query-able form suitable for transmission.
// It holds a type and a value.  The idea is that all MinVR data types
// can be addressed as a MinVRDatum object (well, technically as the
// reference for a MinVRDatumPtr pointer object), for convenient
// loading of heterogeneous types in a homogeneous way, such as in a
// map or list. This class only holds a value; the names must be
// recorded elsewhere.
//
// The data for each object is stored both in its native form, and as
// a serialized data string.  It can be constructed from either form,
// so the serialized string can be sent across the network and
// reconstructed to another datum object on the other side.
//
// To extend the collection of data types that can be modeled as a
// MinVRDatum, follow these steps:
//
//   1. Create a specialization of the MinVRDatum class, and call it
//      something like MinVRDatumInt or MinVRDatumDouble.  You will
//      have to provide it with a private value member and a public
//      getValue() method member.  There are not virtual members for
//      these functions, since their signature differs.
//
//   2. Add a method to the MinVRDatumPtr that will return the new
//      data type.  See intVal() and doubleVal() for models.
//
//   3. Add a "create" function for the factory to use.  Something
//      like these:
//
//        MinVRDatumPtr CreateMinVRDatumInt(void *pData);
//        MinVRDatumPtr CreateMinVRDatumDouble(void *pData);
//
//   4. Add an entry in the MVRTYPE_ID enum above, if necessary.
//
//   5. Then add this number and the create function to the list of
//      data types registered in the constructor for the
//      MinVRDataIndex class.
//
//   6. Add a method to the MinVRDataIndex class that accepts one
//      of the new data types, creates a MinVRDatum object for it,
//      and adds a pointer to it to the data index.
//
//   7. For the configuration functionality, you will need to provide
//      the parser to take the text from the configuration file and
//      translate it into the value and name to be stored here.  This
//      is in MinVRDataIndex::processValue().
//
//
// MinVRDatum is the base class for all supported data types.  In
// addition to supplying an entry for the virtual methods, the
// specializations should also supply a value entry and a getValue()
// method for the appropriate type.  These are not prototyped in the
// base class because they are too variable across the classes and
// templates don't quite seem to handle the range, unless there are
// features of templates that I don't know about, which is probable.
// So it's on you to keep track, not the compiler.
class MinVRDatum {
 protected:
  MVRTYPE_ID type;

  // This is to be a description of the data type, e.g. "int" or
  // "string" or something like that.
  std::string description;

 public:
  // The constructor for the native storage form.
  MinVRDatum(const MVRTYPE_ID inType) : type(inType) {};

  // The constructor for the serialized data form.  This extracts the
  // necessary data from the string, and creates the type, value, and
  // name from there.
  MinVRDatum(const std::string serializedData) {};
  // virtual destructor allows concrete types to implement their own
  // destruction mechanisms
  virtual ~MinVRDatum() {};

  // This produces the serialized version of the datum.  When packaged
  // with the description and a name, this will be ready for
  // transmission across some connection to another process or another
  // machine.
  virtual std::string serialize() = 0;

  // The description of the datum is a part of the network-ready
  // serialized data.
  std::string getDescription() const { return description; };
  MVRTYPE_ID getType() { return type; };
};

/////////// Specializations to handle specific data types.
/////////// This is step 1 in the specialization instructions above.

// This is the specialization for an integer.
class MinVRDatumInt : public MinVRDatum {
private:
  // The actual data is stored here.
  int value;

public:
  MinVRDatumInt(const int inVal);

  std::string serialize();

  int getValue() { return value; };
};

// The specialization for a float.  (Or a 'double' in C++-speak.)
class MinVRDatumDouble : public MinVRDatum {
private:
  double value;

public:
  MinVRDatumDouble(const double inVal);

  std::string serialize();

  double getValue() { return value; };
};

// Specialization for a string
class MinVRDatumString : public MinVRDatum {
private:
  // The actual data is stored here.
  std::string value;

public:
  MinVRDatumString(const std::string inVal);

  std::string serialize();

  std::string getValue() { return value; };
};

// Specialization for a string
class MinVRDatumContainer : public MinVRDatum {
private:
  // The actual data is stored here, a collection of names.
  std::list<std::string> value;

public:
  MinVRDatumContainer(const std::list<std::string> inVal);

  std::string serialize();

  std::list<std::string> getValue() { return value; };
};

// A convenient reference counter for the smart pointer for the MinVRDatum type.
class MinVRDatumPtrRC {
private:
  int count; // Reference count

public:
  MinVRDatumPtrRC(int start) : count(start) {};

  void addRef()
  {
    // Increment the reference count
    count++;
  }

  int release()
  {
    // Decrement the reference count and
    // return the reference count.
    return --count;
  }
};

// A smart pointer class, specialized to work with MinVRDatum.  Note
// that we are overloading the -> operator, so you can use it, but it
// points to a MinVRDatum object.  So if what you want is a field or
// method belonging to MinVRDatumInt or one of the other
// specializations, you have to use one of the specialized accessors.
//
// For example:
//
//  MinVRDatumPtr p;
//
//  <initialize p with integer data via an MinVRDatumInt object>
//
//    p->getValue()          Returns an error, since there is no
//                           getValue() in the MinVRDatum object.
//
//    p.intVal()->getValue() Returns the integer value held in *p.
//
class MinVRDatumPtr {
private:
  MinVRDatum*  pData;         // Pointer
  MinVRDatumPtrRC* reference; // Reference count

public:
  MinVRDatumPtr() : pData(0) { reference = new MinVRDatumPtrRC(1); }
  MinVRDatumPtr(MinVRDatum* pValue) : pData(pValue) {
    reference = new MinVRDatumPtrRC(1);
  }

  MinVRDatumPtr(const MinVRDatumPtr& sp) : pData(sp.pData), reference(sp.reference)
  {
    // Copy constructor: Copy the data and reference pointer and
    // increment the reference count.
    reference->addRef();
  }

  ~MinVRDatumPtr()
  {
    // Destructor: Decrement the reference count.  If reference becomes
    // zero, delete the data
    if(reference->release() == 0)
      {
        delete pData;
      }
  }

  MinVRDatum& operator* ()
  {
    return *pData;
  }

  MinVRDatum* operator-> ()
  {
    return pData;
  }

  MinVRDatumPtr& operator = (const MinVRDatumPtr& sp)
  {
    // Assignment operator
    if (this != &sp) // Avoid self assignment
      {
        // Decrement the old reference count
        // if reference become zero delete the old data
        if(reference->release() == 0)
          {
            delete pData;
          }

        // Copy the data and reference pointer
        // and increment the reference count
        pData = sp.pData;
        reference = sp.reference;
        reference->addRef();
      }
    return *this;
  }

  //////// Specialized accessors below.  See step 2 in the specialization
  //////// instructions above.

  // The power of this pointer is that you can reference any type as
  // any other type.  This is probably more bug than feature, so we have
  // type checks here, and throw an error if it doesn't match.
  MinVRDatumInt* intVal()
  {
    if (pData->getType() == MVRINT) {
      return static_cast<MinVRDatumInt*>(pData);
    } else {
      throw std::runtime_error("This datum is not an int.");
    }
  }

  MinVRDatumDouble* doubleVal()
  {
    if (pData->getType() == MVRFLOAT) {
      return static_cast<MinVRDatumDouble*>(pData);
    } else {
      throw std::runtime_error("This datum is not a float.");
    }
  }

  MinVRDatumString* stringVal()
  {
    if (pData->getType() == MVRSTRING) {
      return static_cast<MinVRDatumString*>(pData);
    } else {
      throw std::runtime_error("This datum is not a string.");
    }
  }

  MinVRDatumContainer* containerVal()
  {
    if (pData->getType() == MVRCONTAINER) {
      return static_cast<MinVRDatumContainer*>(pData);
    } else {
      throw std::runtime_error("This datum is not a container.");
    }
  }

};

// Each specialization needs a callback of the following form, to be
// invoked by the factory.  This is step 3 in the instructions above.
MinVRDatumPtr CreateMinVRDatumInt(void *pData);
MinVRDatumPtr CreateMinVRDatumDouble(void *pData);
MinVRDatumPtr CreateMinVRDatumString(void *pData);
MinVRDatumPtr CreateMinVRDatumContainer(void *pData);


#endif
