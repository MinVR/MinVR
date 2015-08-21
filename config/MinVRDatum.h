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
//
// Right now we just store values as strings and be done with it, but
// that's just a crutch to allow us to work on the settings business.
// It might deserve serious consideration as an option, but that
// hasn't happened as of 8/19.
//
// The base class for all supported data types.
// In addition to supplying an entry for the virtual methods, the
// specializations should also supply a value entry and a getValue()
// method for the appropriate type.
class MinVRDatum {
 protected:
  MVRTYPE_ID type;
  std::string name;
 public:

  MinVRDatum(const MVRTYPE_ID inType, const std::string inName) :
    type(inType), name(inName) {};
  MinVRDatum(const MVRTYPE_ID inType, const char* inName) :
    type(inType), name(std::string(inName)) {};
  // virtual destructor allows concrete types to implement their own
  // destruction mechanisms
  virtual ~MinVRDatum() {}

  virtual std::string serialize() = 0;
  virtual std::string getDescription() = 0;

  virtual void doSomething() const = 0;
};

// create some concrete shapes... you would do this in other CPP files
class MinVRDatumInt : public MinVRDatum {
private:
  int value;

public:
  MinVRDatumInt(const MVRTYPE_ID inType, const int inVal, const std::string inName) :
    MinVRDatum(inType, inName), value(inVal) {};
  MinVRDatumInt(const MVRTYPE_ID inType, const int inVal, const char* inName) :
    MinVRDatum(inType, inName), value(inVal) {};

  void doSomething() const;
  std::string serialize();
  std::string getDescription();
};

// another concrete shape...
class MinVRDatumDouble : public MinVRDatum {
private:
  double value;

public:
  MinVRDatumDouble(const MVRTYPE_ID inType,
                   const double inVal,
                   const std::string inName) :
    MinVRDatum(inType, inName), value(inVal) {};
  MinVRDatumDouble(const MVRTYPE_ID inType,
                   const double inVal,
                   const char* inName) :
    MinVRDatum(inType, inName), value(inVal) {};



  void doSomething() const;
  std::string serialize();
  std::string getDescription();
};
// ... other concrete shapes...

MinVRDatum* CreateMinVRDatumDouble();
MinVRDatum* CreateMinVRDatumInt();



#endif
