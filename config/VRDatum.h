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

/// This is the list of data types we can handle.  This is step 1
/// in the specialization instructions below.
typedef enum
{
  MVRNONE       = 0,
  MVRINT        = 1,
  MVRDOUBLE      = 2,
  MVRSTRING     = 3,
  MVRARRAYINT     = 4,
  MVRARRAYDOUBLE   = 5,
  MVRARRAYSTRING  = 6,
  MVRCONTAINER  = 7
} MVRTYPE_ID;

#define MVRNTYPES 7

typedef int MVRInt;
typedef double MVRDouble;
typedef std::string MVRString;
// An MVRContainer is actually a list of strings.
typedef std::list<std::string> MVRContainer;
typedef std::vector<double> MVRArrayDouble;

// This class is a helper to avoid having to access values with
// constructs like ptr.intVal()->getValue().  By using this helper
// class, that redefines the type conversion operators, we can do
// things like this:
//
//   int p = ptr->getValue();
//
// and have the getValue() function return a value of type int.  Or
//
//   double p = ptr->getValue();
//
// and get a double back in return.  The idea is to absorb as much of
// the pain of casts and type conversions and not force the programmer
// to deal with it.  I know this isn't the C++ ethos, but hey, call me
// a dreamer.
//
// The template part of this is just there to avoid a circular
// reference.  That is, the VRDatumPtr::getValue() will return one
// of these values, that in turn depends on VRDatumPtr.  So we
// define it as a template here, and fill out the values inside the
// definition of VRDatumPtr.
template <typename T>
class VRDatumHelper {

  T const* datum;

public:
  VRDatumHelper(T const* inDatum) : datum(inDatum) {};

  // Adding a data type?  Add a corresponding redefinition of the
  // appropriate type conversion operator here.  Step 2 in the
  // add-a-data-type instructions.
  operator int() const { return datum->getValueInt(); }
  operator double() const { return datum->getValueDouble(); }
  operator std::string() const { return datum->getValueString(); }
  operator MVRContainer() const { return datum->getValueContainer(); }
  operator MVRArrayDouble() const { return datum->getValueArrayDouble(); }
};

// This class is meant to hold a data object of some arbitrary type
// (ok, arbitrary within a small range of types), and to serialize and
// deserialize it into a query-able form suitable for transmission.
// It holds a type and a value.  The idea is that all VR data types
// can be addressed as a VRDatum object (well, technically as the
// reference for a VRDatumPtr pointer object), for convenient
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
// VRDatum, follow these steps:
//
//   1. Add an entry in the MVRTYPE_ID enum above, if necessary, and
//      the initialization of the MVRTypeMap in VRDatum.cpp.
//
//   2.  Add a conversion to the VRDatumHelper class.  You'll
//       probably want to make your type a typedef if you haven't
//       already.  Put it up there with MVRContainer.
//
//   3. Create a specialization of the VRDatum class, and call it
//      something like VRDatumInt or VRDatumDouble.  You will
//      have to provide it with a private value member and public
//      getValue*() and getValue() method members.  (The latter are
//      all the same, so that's no big deal.)  Add an entry to the list of
//      virtual members for these functions, too.
//
//   4.  You will also need a setValue() function.  These objects are
//       meant to be immutable, but when a new name-value pair has the
//       same name as another pair it seems a shame to create an
//       entirely new object to hold the new value when an
//       already-existing one is right there.  So when it is
//       appropriate for a new name to supplant the old, we can
//       provide some small degree of optimization by just changing
//       the value of the object.  So these objects are only sort of
//       immutable in the sense that we're supposed to pretend they
//       are, and so long as no one lets on, the secret will be safe.
//       Ok?  The syntax for using setValue() is annoying, so users
//       shouldn't really be using it anyway.
//
//   5. Add a method to the VRDatumPtr that will return the new
//      data type.  See intVal() and doubleVal() for models.
//
//   6. Add a "create" function for the factory to use.  Something
//      like these:
//
//        VRDatumPtr CreateVRDatumInt(void *pData);
//        VRDatumPtr CreateVRDatumDouble(void *pData);
//
//   7. Then add this number and the create function to the list of
//      data types registered in the constructor for the
//      VRDataIndex class.
//
//   8. Add an addValue() method to the VRDataIndex class that
//      accepts one of the new data types, creates a VRDatum object
//      for it, and adds a pointer to it in the data index.
//
//   9. Write a deserialize*() method for VRDataCollection and find
//      a place for it in the processValue() method of that object.
//
//   10. You should also consider adding something to
//      VRDataIndex::inferType to identify the new data type.  This
//      is pretty free-form, but follow the models in there.  This is
//      also not necessary, but if you don't do it, your data types
//      will always require a 'type="XX"' attribute when specified,
//      and may not give obvious errors if you don't do that.
//
//
// VRDatum is the base class for all supported data types.  In
// addition to supplying an entry for the virtual methods, the
// specializations should also supply a value entry and a getValue()
// method for the appropriate type.  These are not prototyped in the
// base class because they are too variable across the classes and
// templates don't quite seem to handle the range, unless there are
// features of templates that I don't know about, which is probable.
// So it's on you to keep track, not the compiler.
class VRDatum {
 protected:
  MVRTYPE_ID type;

  // This is to be a description of the data type, e.g. "int" or
  // "string" or something like that.
  std::string description;

  //friend std::ostream & operator<<(std::ostream &os, const VRDatum& p);

 public:
  // The constructor for the native storage form.
  VRDatum(const MVRTYPE_ID inType) : type(inType) {};

  // The constructor for the serialized data form.  This extracts the
  // necessary data from the string, and creates the type, value, and
  // name from there.
  VRDatum(const std::string serializedData) {};
  // virtual destructor allows concrete types to implement their own
  // destruction mechanisms.  Specifically, types that involve
  // pointers should be careful to delete their objects.
  virtual ~VRDatum() {};

  // This array is a mapping between MVRTYPE_ID and the string
  // description of that type that will appear in serialized data.  It
  // is here as a public member as a convenience for other classes
  // that will need this mapping (e.g. VRDatumFactory and
  // VRDataCollection).
  typedef struct { std::string first; MVRTYPE_ID second; } MVRTypePair;
  static const MVRTypePair MVRTypeMap[MVRNTYPES];
  std::string initializeDescription(MVRTYPE_ID t);

  // This produces the serialized version of the datum.  When packaged
  // with the description and a name, this will be ready for
  // transmission across some connection to another process or another
  // machine.
  virtual std::string serialize() = 0;

  // The description of the datum is a part of the network-ready
  // serialized data.  It's in the 'type=""' part of the XML.
  std::string getDescription() const { return description; };
  MVRTYPE_ID getType() { return type; };

  // The generic getValue() method returns an object of the helper
  // class above, which is then coerced into the type the user
  // actually wants.
  virtual VRDatumHelper<VRDatum> getValue() = 0;

  // Less generic getValue methods.  One of these is to be overridden
  // in each specialization of this class.  The others are here to
  // prevent bad behavior, and throw an error if the programmer
  // attempts to coerce a data type incorrectly.
  virtual int getValueInt() const {
    throw std::runtime_error("This datum is not an int.");
  }
  virtual double getValueDouble() const {
    throw std::runtime_error("This datum is not a double.");
  }
  virtual std::string getValueString() const {
    throw std::runtime_error("This datum is not a std::string.");
  }
  virtual MVRArrayDouble getValueArrayDouble() const {
    throw std::runtime_error("This datum is not a std::string.");
  }
  virtual MVRContainer getValueContainer() const {
    throw std::runtime_error("This datum is not a container.");
  }
};

/////////// Specializations to handle specific data types.  This is
/////////// step 3 in the specialization instructions above.  Note
/////////// that each one of these overrides one of the virtual
/////////// accessors above, so make sure that list is complete, too.

// This is the specialization for an integer.
class VRDatumInt : public VRDatum {
private:
  // The actual data is stored here.
  int value;

public:
  VRDatumInt(const int inVal);

  std::string serialize();

  int getValueInt() const { return value; };
  bool setValue(const int inVal);

  VRDatumHelper<VRDatum> getValue() {
    return VRDatumHelper<VRDatum>(this);
  }
};

// The specialization for a double.
class VRDatumDouble : public VRDatum {
private:
  double value;

public:
  VRDatumDouble(const double inVal);

  std::string serialize();

  double getValueDouble() const { return value; };
  bool setValue(const double inVal);

  VRDatumHelper<VRDatum> getValue() {
    return VRDatumHelper<VRDatum>(this);
  }
};

// Specialization for a string
class VRDatumString : public VRDatum {
private:
  // The actual data is stored here.
  std::string value;

public:
  VRDatumString(const std::string inVal);

  std::string serialize();

  std::string getValueString() const { return value; };
  bool setValue(const std::string inVal);

  VRDatumHelper<VRDatum> getValue() {
    return VRDatumHelper<VRDatum>(this);
  }
};

// Specialization for a vector of doubles
class VRDatumArrayDouble : public VRDatum {
private:
  // The actual data is stored here.
  MVRArrayDouble value;

public:
  VRDatumArrayDouble(const std::vector<double> inVal);

  std::string serialize();

  MVRArrayDouble getValueArrayDouble() const { return value; };
  bool setValue(const std::vector<double> inVal);

  VRDatumHelper<VRDatum> getValue() {
    return VRDatumHelper<VRDatum>(this);
  }
};

// Specialization for a container
class VRDatumContainer : public VRDatum {
private:
  // The actual data is stored here, a collection of names.
  MVRContainer value;

public:
  VRDatumContainer(const MVRContainer inVal);

  std::string serialize();

  MVRContainer getValueContainer() const { return value; };
  bool addToValue(const MVRContainer inVal);

  VRDatumHelper<VRDatum> getValue() {
    return VRDatumHelper<VRDatum>(this);
  }
};


// A convenient reference counter for the smart pointer for the VRDatum type.
class VRDatumPtrRC {
private:
  int count; // Reference count

public:
  VRDatumPtrRC(int start) : count(start) {};

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

// A smart pointer class, specialized to work with VRDatum.  Note
// that we are overloading the -> operator, so you can use it, but it
// points to a VRDatum object.  So if what you want is a field or
// method belonging to VRDatumInt or one of the other
// specializations, you have to use one of the specialized accessors.
//
// For example:
//
//  VRDatumPtr p;
//
//  <initialize p with integer data via an VRDatumInt object>
//
//    p.intVal()->getValue() Returns the integer value held in *p.
//
//    p->getValue()          Nicer syntax, gets you the same value,
//                           so long as there is some kind of cast
//                           or assignment to determine a type.
//
// However, p->getDescription() works fine, as will p->serialize(),
// and other things that are actually part of the VRDatum core
// class.
//
class VRDatumPtr {
private:
  VRDatum*  pData;         // Pointer
  VRDatumPtrRC* reference; // Reference count

  //friend std::ostream & operator<<(std::ostream &os, const VRDatumPtr& p);

public:
  VRDatumPtr() : pData(0) { reference = new VRDatumPtrRC(1); }
  VRDatumPtr(VRDatum* pValue) : pData(pValue) {
    reference = new VRDatumPtrRC(1);
  }

  VRDatumPtr(const VRDatumPtr& sp) : pData(sp.pData), reference(sp.reference)
  {
    // Copy constructor: Copy the data and reference pointer and
    // increment the reference count.
    reference->addRef();
  }

  ~VRDatumPtr()
  {
    // Destructor: Decrement the reference count.  If reference becomes
    // zero, delete the data
    if(reference->release() == 0)
      {
        delete pData;
      }
  }

  VRDatum& operator* ()
  {
    return *pData;
  }

  VRDatum* operator-> ()
  {
    return pData;
  }

  VRDatumPtr& operator = (const VRDatumPtr& sp)
  {
    // Assignment operator
    if (this != &sp) // Avoid self assignment
      {
        // Decrement the old reference count.  If references become
        // zero, delete the data.
        if(reference->release() == 0)
          {
            delete pData;
          }

        // Copy the data and reference pointer and increment the
        // reference count
        pData = sp.pData;
        reference = sp.reference;
        reference->addRef();
      }
    return *this;
  }

  //////// Specialized accessors below.  See step 5 in the specialization
  //////// instructions above.

  // The power of this pointer is that you can reference any type as
  // any other type.  This is probably more bug than feature, so we
  // have type checks here, and throw an error if it doesn't match.
  // This is also easier to use (my opinion) than trying to remember
  // how to do the casts.
  VRDatumInt* intVal()
  {
    if (pData->getType() == MVRINT) {
      return static_cast<VRDatumInt*>(pData);
    } else {
      throw std::runtime_error("This datum is not an int.");
    }
  }

  VRDatumDouble* doubleVal()
  {
    if (pData->getType() == MVRDOUBLE) {
      return static_cast<VRDatumDouble*>(pData);
    } else {
      throw std::runtime_error("This datum is not a double.");
    }
  }

  VRDatumString* stringVal()
  {
    if (pData->getType() == MVRSTRING) {
      return static_cast<VRDatumString*>(pData);
    } else {
      throw std::runtime_error("This datum is not a string.");
    }
  }

  VRDatumArrayDouble* arrayDoubleVal()
  {
    if (pData->getType() == MVRARRAYDOUBLE) {
      return static_cast<VRDatumArrayDouble*>(pData);
    } else {
      throw std::runtime_error("This datum is not an array of doubles.");
    }
  }

  VRDatumContainer* containerVal()
  {
    if (pData->getType() == MVRCONTAINER) {
      return static_cast<VRDatumContainer*>(pData);
    } else {
      throw std::runtime_error("This datum is not a container.");
    }
  }

};

// Each specialization needs a callback of the following form, to be
// invoked by the factory.  This is step 6 in the instructions above.
VRDatumPtr CreateVRDatumInt(void *pData);
VRDatumPtr CreateVRDatumDouble(void *pData);
VRDatumPtr CreateVRDatumString(void *pData);
VRDatumPtr CreateVRDatumArrayDouble(void *pData);
VRDatumPtr CreateVRDatumContainer(void *pData);


#endif
