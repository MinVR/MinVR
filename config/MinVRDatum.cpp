#include "MinVRDatum.h"

MinVRDatumInt::MinVRDatumInt(const int inVal) :
  MinVRDatum(MVRINT), value(inVal) {
  description = "int";
};

bool MinVRDatumInt::setValue(const int inVal) {
  value = inVal;
  return true;
}

std::string MinVRDatumInt::serialize() {
  char buffer[20];
  sprintf(buffer, "%d", value);
  return std::string(buffer);
}

MinVRDatumPtr CreateMinVRDatumInt(void *pData) {
  MinVRDatumInt *obj = new MinVRDatumInt(*static_cast<int *>(pData));
  return MinVRDatumPtr(obj);
}

////////////////////////////////////////////

MinVRDatumDouble::MinVRDatumDouble(const double inVal) :
  MinVRDatum(MVRFLOAT), value(inVal) {
  description = "float";
};

bool MinVRDatumDouble::setValue(const double inVal) {
  value = inVal;
  return true;
}

std::string MinVRDatumDouble::serialize() {
  char buffer[20];
  sprintf(buffer, "%f", value);
  return std::string(buffer);
}

MinVRDatumPtr CreateMinVRDatumDouble(void *pData) {
  MinVRDatumDouble *obj = new MinVRDatumDouble(*static_cast<double *>(pData));
  return MinVRDatumPtr(obj);
}

////////////////////////////////////////////

MinVRDatumString::MinVRDatumString(const std::string inVal) :
  MinVRDatum(MVRSTRING), value(inVal) {
  description = "string";
};

bool MinVRDatumString::setValue(const std::string inVal) {
  value = inVal;
  return true;
}

std::string MinVRDatumString::serialize() {
  return getValue();
}

MinVRDatumPtr CreateMinVRDatumString(void *pData) {
  MinVRDatumString *obj = new MinVRDatumString(*static_cast<std::string *>(pData));
  return MinVRDatumPtr(obj);
}

////////////////////////////////////////////

MinVRDatumContainer::MinVRDatumContainer(const std::list<std::string> inVal) :
  MinVRDatum(MVRCONTAINER), value(inVal) {
  description = "container";
};

// For optimization and code maintainability reasons, the
// responsibility for assembling the serialization of a container
// falls to the index class, so this function is sort of a nop, filled
// out just to keep the compiler happy.
std::string MinVRDatumContainer::serialize() {
  throw std::runtime_error(std::string("shouldn't call the serialize() method of a container object."));
  return getDescription();
}

bool MinVRDatumContainer::addToValue(const std::list<std::string> inVal) {
  std::list<std::string> inCopy = inVal;

  // Remove all duplicates from the input list.
  for (std::list<std::string>::const_iterator it = value.begin();

       it != value.end(); ++it) {
    inCopy.remove(*it);
  }
  value.splice(value.end(), inCopy);
  return true;
}

MinVRDatumPtr CreateMinVRDatumContainer(void *pData) {
  MinVRDatumContainer *obj = new MinVRDatumContainer(*static_cast<std::list<std::string> *>(pData));
  return MinVRDatumPtr(obj);
}



/*
std::ostream & operator<<(std::ostream &os, const MinVRDatum& p)
{
    return os << p.to_str();
}
*/
