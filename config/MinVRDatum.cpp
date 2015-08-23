#include "MinVRDatum.h"

MinVRDatumInt::MinVRDatumInt(const int inVal) :
  MinVRDatum(MVRINT), value(inVal) {
  description = "int";
};



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


std::string MinVRDatumString::serialize() {
  return getValue();
}

MinVRDatumPtr CreateMinVRDatumString(void *pData) {
  MinVRDatumString *obj = new MinVRDatumString(*static_cast<std::string *>(pData));
  return MinVRDatumPtr(obj);
}

////////////////////////////////////////////

MinVRDatumContainer::MinVRDatumContainer(const std::string inVal) :
  MinVRDatum(MVRCONTAINER), value(inVal) {
  description = "container";
};


std::string MinVRDatumContainer::serialize() {
  return getValue();
}

MinVRDatumPtr CreateMinVRDatumContainer(void *pData) {
  MinVRDatumContainer *obj = new MinVRDatumContainer(*static_cast<std::string *>(pData));
  return MinVRDatumPtr(obj);
}



/*
std::ostream & operator<<(std::ostream &os, const MinVRDatum& p)
{
    return os << p.to_str();
}
*/
