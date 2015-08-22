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

void MinVRDatumInt::doSomething() const {
  std::cout << "Something with an int " << " (" << ")" << std::endl;
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

void MinVRDatumDouble::doSomething() const {
  std::cout << "Something with a double " << " (" << ")" << std::endl;
}

MinVRDatumPtr CreateMinVRDatumDouble(void *pData) {
  MinVRDatumDouble *obj = new MinVRDatumDouble(*static_cast<double *>(pData));
  return MinVRDatumPtr(obj);
}



/*
std::ostream & operator<<(std::ostream &os, const MinVRDatum& p)
{
    return os << p.to_str();
}
*/
