// -*-c++-*-
#ifndef MINVR_DATUM_H
#define MINVR_DATUM_H

#include <map>
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
// It holds a type, value, and a name.  The idea is that all MinVR
// data types can be addressed as a MinVRDatum object, for convenient
// loading in a map or list.
//
// The data for each object is stored both in its native form, and as
// a serialized data string.  It can be constructed from either form,
// so the serialized string can be sent across the network and
// reconstructed to another datum object on the other side.


// This is the base class for all supported data types.  In addition
// to supplying an entry for the virtual methods, the specializations
// should also supply a value entry and a getValue() method for the
// appropriate type.  These are not prototyped in the base class
// because they are too variable across the classes.  It's on you to
// keep track, not the compiler.
class MinVRDatum {
 protected:
  MVRTYPE_ID type;
  std::string name;

  // This is to be a description of the data type, e.g. "int nWindows" or
  // "string winName" or something like that.
  std::string description;

 public:
  // The constructor for the native storage form.
  MinVRDatum(const MVRTYPE_ID inType, const std::string inName) :
    type(inType), name(inName) {};

  // The constructor for the serialized data form.  This extracts the
  // necessary data from the string, and creates the type, value, and
  // name from there.
  MinVRDatum(const std::string serializedData) {};
  // virtual destructor allows concrete types to implement their own
  // destruction mechanisms
  virtual ~MinVRDatum() {}

  // This produces the serialized version of the datum, ready for
  // transmission across some connection to another process or another
  // machine.
  virtual std::string serialize() = 0;

  std::string getDescription() { return description; };

  virtual void doSomething() const = 0;
};

// This is the specialization for an integer.
class MinVRDatumInt : public MinVRDatum {
private:
  // The actual data is stored here.
  int value;

public:
  MinVRDatumInt(const int inVal, const std::string inName);

  void doSomething() const;
  std::string serialize();
  std::string getDescription();
};

// The specialization for a float.  (Or a 'double' in C++-speak.)
class MinVRDatumDouble : public MinVRDatum {
private:
  double value;

public:
  MinVRDatumDouble(const double inVal,
                   const std::string inName);

  void doSomething() const;
  std::string serialize();
  std::string getDescription();
};

// Each specialization needs a callback of the following form, to be
// invoked by the factory.
MinVRDatum* CreateMinVRDatumInt(void *pData, std::string name);
MinVRDatum* CreateMinVRDatumDouble(void *pData, std::string name);

#endif
