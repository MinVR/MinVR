// -*-c++-*-
#ifndef MINVR_DATUM_H
#define MINVR_DATUM_H

#include "VRCoreTypes.h"

#include <map>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <sstream>

// This is the default separator used to separate values in a
// serialized vector.  There is a 'separator=' attribute in the
// serialized (XML) form that can override.
#define MINVRSEPARATOR '@'

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
class VRDatumConverter {

  T const* datum;

public:
  VRDatumConverter(T const* inDatum) : datum(inDatum) {};

  // Adding a data type?  Add a corresponding redefinition of the
  // appropriate type conversion operator here.  Step 2 in the
  // add-a-data-type instructions.
  operator VRInt() const { return datum->getValueInt(); }
  operator VRDouble() const { return datum->getValueDouble(); }
  operator VRString() const { return datum->getValueString(); }
  operator VRIntArray() const { return datum->getValueIntArray(); }
  operator VRDoubleArray() const { return datum->getValueDoubleArray(); }
  operator VRStringArray() const { return datum->getValueStringArray(); }
  operator VRContainer() const { return datum->getValueContainer(); }
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
//   1. Add an entry in the VRCORETYPE_ID enum in VRCoreTypes.h, if
//      necessary, and the initialization of the mVRTypeMap in
//      VRDatum.cpp.
//
//   2. Add a conversion to the VRDatumConverter class.  You'll probably
//      want to make your type a typedef if you haven't already.  Put
//      it up there with VRContainer.
//
//   3. Create a specialization of the VRDatum class, and call it
//      something like VRDatumInt or VRDatumDouble.  You will
//      have to provide it with a private value member and public
//      getValue*() and getValue() method members.  (The latter are
//      all the same, so that's no big deal.)  Add an entry to the list of
//      virtual members for these functions, too.
//
//   4. You will also need a setValue() function.  These objects are
//      meant to be immutable, but when a new name-value pair has the
//      same name as another pair it seems a shame to create an
//      entirely new object to hold the new value when an
//      already-existing one is right there.  So when it is
//      appropriate for a new name to supplant the old, we can provide
//      some small degree of optimization by just changing the value
//      of the object.  So these objects are only sort of immutable in
//      the sense that we're supposed to pretend they are, and so long
//      as no one lets on, the secret will be safe.  Ok?  The syntax
//      for using setValue() is annoying, so users shouldn't really be
//      using it anyway.
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
//   8. Add an addData() method to the VRDataIndex class that
//      accepts one of the new data types, creates a VRDatum object
//      for it, and adds a pointer to it in the data index.
//
//   9. Write a deserialize*() method for VRDataIndex and find
//      a place for it in the processValue() method of that object.
//
//   10.You should also consider adding something to
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
public:
  typedef std::map<std::string, std::string> VRAttributeList;

protected:
  VRCORETYPE_ID type;

  // This is to be a description of the data type, e.g. "int" or
  // "string" or something like that.
  std::string description;

  std::list<VRAttributeList> attrList;

  //friend std::ostream & operator<<(std::ostream &os, const VRDatum& p);

public:
  // The constructor for the native storage form.
  VRDatum(const VRCORETYPE_ID inType)
    : type(inType) {
    attrList.push_front(VRAttributeList());
  };

  // virtual destructor allows concrete types to implement their own
  // destruction mechanisms.  Specifically, types that involve
  // pointers should be careful to delete their objects.
  virtual ~VRDatum() {};

  // Some facilities for handling XML attributes within the class.
  // The 'type=' attribute is the only one that really matters to the
  // operation of the class (so is *not* stored in the attribute
  // list), but other attributes might matter to other applications.
  // There is also a 'separator=' attribute that indicates a character
  // to use in the serialized version of an array.
  VRAttributeList getAttributeList() { return attrList.front(); };
  void setAttributeList(VRAttributeList newList) { attrList.front() = newList; };
  std::string getAttributeValue(const std::string attributeName) {
    return attrList.front()[attributeName];
  }
  void setAttributeValue(const std::string attributeName,
                         const std::string attributeValue) {
    attrList.front()[attributeName] = attributeValue;
  }
  // Returns the attribute list formatted to include in an XML tag.
  std::string getAttributeListAsString();
  
  // This array is a mapping between VRCORETYPE_ID and the string
  // description of that type that will appear in serialized data.  It
  // is here as a public member as a convenience for other classes
  // that will need this mapping (e.g. VRDatumFactory and
  // VRDataIndex).
  typedef struct { std::string first; VRCORETYPE_ID second; } VRTypePair;
  static const VRTypePair VRTypeMap[VRCORETYPE_NTYPES];
  std::string initializeDescription(VRCORETYPE_ID t);

  // This produces the serialized version of the datum.  When packaged
  // with the description and a name, this will be ready for
  // transmission across some connection to another process or another
  // machine.
  virtual std::string getValueAsString() = 0;

  // The description of the datum is a part of the network-ready
  // serialized data.  It's in the 'type=""' part of the XML.
  std::string getDescription() const { return description; };
  VRCORETYPE_ID getType() { return type; };

  // The generic getValue() method returns an object of the helper
  // class above, which is then coerced into the type the user
  // actually wants.
  virtual VRDatumConverter<VRDatum> getValue() = 0;

  // The easiest way to accommodate the push/pop feature of
  // VRDatumSpecialized, below.
  virtual void push() = 0;
  virtual bool pop() = 0;
  
  // Less generic getValue methods.  One of these is to be overridden
  // in each specialization of this class.  The others are here to
  // prevent bad behavior, and throw an error if the programmer
  // attempts to coerce a data type incorrectly.
  virtual VRInt getValueInt() const {
    throw std::runtime_error("This datum is not a VRInt.");
  }
  virtual VRDouble getValueDouble() const {
    throw std::runtime_error("This datum is not a VRDouble.");
  }
  virtual VRString getValueString() const {
    throw std::runtime_error("This datum is not a VRString.");
  }
  virtual VRIntArray getValueIntArray() const {
    throw std::runtime_error("This datum is not a VRIntArray.");
  }
  virtual VRDoubleArray getValueDoubleArray() const {
    throw std::runtime_error("This datum is not a VRDoubleArray.");
  }
  virtual VRStringArray getValueStringArray() const {
    throw std::runtime_error("This datum is not a VRStringArray.");
  }
  virtual VRContainer getValueContainer() const {
    throw std::runtime_error("This datum is not a VRContainer.");
  }
};

typedef VRDatumConverter<VRDatum> VRAnyCoreType;

/////////// Specializations to handle specific data types.  This is
/////////// step 3 in the specialization instructions above.  Note
/////////// that each one of these overrides one of the virtual
/////////// accessors above, so make sure that list is complete, too.

template <class T, const VRCORETYPE_ID TID>
class VRDatumSpecialized : public VRDatum {
protected:
  // The actual data is stored here.  It is stored as a std::list, so
  // that we can push context frames onto the stack.
  std::list<T> value;

  bool needPush, pushed;
  int stackFrame;
  
public:
  VRDatumSpecialized(const T inVal):
    VRDatum(TID), needPush(false), pushed(false), stackFrame(1) {
    value.push_front(inVal);
    description = initializeDescription(type);
  };

  // Couldn't figure out how to template-ize this one.
  virtual std::string getValueAsString() = 0;

  bool setValue(const T inVal) {
    // This is a little optimization.  You only need to push things
    // onto the stack if the value actually changes.
    if (needPush) {
      value.push_front( value.front() );
      attrList.push_front( attrList.front() );
      needPush = false;
      pushed = true;
    }
    value.front() = inVal;
    return true;
  }
  
  VRDatumConverter<VRDatum> getValue() {
    return VRDatumConverter<VRDatum>(this);
  }

  // These two methods allow us to "remember" a state of the data
  // index.  The "push" function pushes a copy of the current state
  // onto the (imaginary) stack.  You can modify and mess with the
  // state in any way you like, and restore the original values by
  // invoking the "pop" method.
  //
  // After a push(), we only really push a new value onto the stack
  // when someone tries to change the old one.  So we only have to pop
  // it when it has been modified.
  void push() { needPush = true; stackFrame++; };
  bool pop() {
    stackFrame--;
    if (pushed && (stackFrame > 0)) {
      value.pop_front();
      attrList.pop_front();
      pushed = false;
    };
    // A value with a stackFrame zero or less should be cleaned up.
    // It would have been added to the data index in a context that no
    // longer exists.  Use the return value from this function to
    // indicate a datum should be cleaned up.
    return (stackFrame <= 0);
  };
};

// This is the specialization for an integer.
class VRDatumInt : public VRDatumSpecialized<VRInt, VRCORETYPE_INT> {
public:
  VRDatumInt(const VRInt inVal) :
    VRDatumSpecialized<VRInt, VRCORETYPE_INT>(inVal) {};
  std::string getValueAsString();
  VRInt getValueInt() const { return value.front(); };
};

// The specialization for a double.
class VRDatumDouble : public VRDatumSpecialized<VRDouble, VRCORETYPE_DOUBLE> {
public:
  VRDatumDouble(const VRDouble inVal) :
    VRDatumSpecialized<VRDouble, VRCORETYPE_DOUBLE>(inVal) {};
  std::string getValueAsString();
  VRDouble getValueDouble() const { return value.front(); };
};

// Specialization for a string
class VRDatumString : public VRDatumSpecialized<VRString, VRCORETYPE_STRING> {
public:
  VRDatumString(const VRString inVal) :
    VRDatumSpecialized<VRString, VRCORETYPE_STRING>(inVal) {};
  std::string getValueAsString();
  VRString getValueString() const { return value.front(); };
};

// Specialization for a vector of ints
class VRDatumIntArray : public VRDatumSpecialized<VRIntArray, VRCORETYPE_INTARRAY> {
public:
  VRDatumIntArray(const VRIntArray inVal) :
    VRDatumSpecialized<VRIntArray, VRCORETYPE_INTARRAY>(inVal) {};
  std::string getValueAsString();
  VRIntArray getValueIntArray() const { return value.front(); };
};

// Specialization for a vector of doubles
class VRDatumDoubleArray : public VRDatumSpecialized<VRDoubleArray, VRCORETYPE_DOUBLEARRAY> {
public:
  VRDatumDoubleArray(const VRDoubleArray inVal) :
    VRDatumSpecialized<VRDoubleArray, VRCORETYPE_DOUBLEARRAY>(inVal) {};
  std::string getValueAsString();
  VRDoubleArray getValueDoubleArray() const { return value.front(); };
};

// Specialization for a vector of strings
class VRDatumStringArray : public VRDatumSpecialized<VRStringArray, VRCORETYPE_STRINGARRAY> {
public:
  VRDatumStringArray(const VRStringArray inVal) :
    VRDatumSpecialized<VRStringArray, VRCORETYPE_STRINGARRAY>(inVal) {};
  std::string getValueAsString();
  VRStringArray getValueStringArray() const { return value.front(); };
};

// Specialization for a container
class VRDatumContainer : public VRDatumSpecialized<VRContainer, VRCORETYPE_CONTAINER> {
public:
  VRDatumContainer(const VRContainer inVal) :
    VRDatumSpecialized<VRContainer, VRCORETYPE_CONTAINER>(inVal) {};
  std::string getValueAsString();
  VRContainer getValueContainer() const { return value.front(); };

  bool addToValue(const VRContainer inVal);

  // In the context of the VRDataIndex, pop is a more complex
  // operation for a container than the pop() for the other data types
  // because we have to go through and delete any objects that were
  // added since the push().  This means comparing the old and new
  // lists and deleting the oddballs.
  VRContainer popAndClean();
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
// However, p->getDescription() works fine, as will p->getValueAsString(),
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
		delete reference;
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
			delete reference;
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
    if (pData->getType() == VRCORETYPE_INT) {
      return static_cast<VRDatumInt*>(pData);
    } else {
      throw std::runtime_error("This datum is not a VRInt.");
    }
  }

  VRDatumDouble* doubleVal()
  {
    if (pData->getType() == VRCORETYPE_DOUBLE) {
      return static_cast<VRDatumDouble*>(pData);
    } else {
      throw std::runtime_error("This datum is not a VRDouble.");
    }
  }

  VRDatumString* stringVal()
  {
    if (pData->getType() == VRCORETYPE_STRING) {
      return static_cast<VRDatumString*>(pData);
    } else {
      throw std::runtime_error("This datum is not a VRString.");
    }
  }

  VRDatumIntArray* intArrayVal()
  {
    if (pData->getType() == VRCORETYPE_INTARRAY) {
      return static_cast<VRDatumIntArray*>(pData);
    } else {
      throw std::runtime_error("This datum is not an array of ints.");
    }
  }

  VRDatumDoubleArray* doubleArrayVal()
  {
    if (pData->getType() == VRCORETYPE_DOUBLEARRAY) {
      return static_cast<VRDatumDoubleArray*>(pData);
    } else {
      throw std::runtime_error("This datum is not an array of doubles.");
    }
  }

  VRDatumStringArray* stringArrayVal()
  {
    if (pData->getType() == VRCORETYPE_STRINGARRAY) {
      return static_cast<VRDatumStringArray*>(pData);
    } else {
      throw std::runtime_error("This datum is not an array of strings.");
    }
  }

  VRDatumContainer* containerVal()
  {
    if (pData->getType() == VRCORETYPE_CONTAINER) {
      return static_cast<VRDatumContainer*>(pData);
    } else {
      throw std::runtime_error("This datum is not a VRContainer.");
    }
  }

};

// Each specialization needs a callback of the following form, to be
// invoked by the factory.  This is step 6 in the instructions above.
VRDatumPtr CreateVRDatumInt(void *pData);
VRDatumPtr CreateVRDatumDouble(void *pData);
VRDatumPtr CreateVRDatumString(void *pData);
VRDatumPtr CreateVRDatumIntArray(void *pData);
VRDatumPtr CreateVRDatumDoubleArray(void *pData);
VRDatumPtr CreateVRDatumStringArray(void *pData);
VRDatumPtr CreateVRDatumContainer(void *pData);


#endif
