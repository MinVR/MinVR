#include "MinVRDatum.h"

MinVRDatumInt::MinVRDatumInt(const int inVal) :
  MinVRDatum(MVRINT), value(inVal) {
  description = "int";
};



std::string MinVRDatumInt::serialize() {
  return std::string("hello");
}

void MinVRDatumInt::doSomething() const {
  std::cout << "Something with an int " << " (" << ")" << std::endl;
}

MinVRDatum* CreateMinVRDatumInt(void *pData) {
  return new MinVRDatumInt(*static_cast<int *>(pData));
}

////////////////////////////////////////////

MinVRDatumDouble::MinVRDatumDouble(const double inVal) :
  MinVRDatum(MVRFLOAT), value(inVal) {
  description = "float";
};


std::string MinVRDatumDouble::serialize() {
  return std::string("good-bye");
}

void MinVRDatumDouble::doSomething() const {
  std::cout << "Something with a double " << " (" << ")" << std::endl;
}

MinVRDatum* CreateMinVRDatumDouble(void *pData) {
  return new MinVRDatumDouble(*static_cast<double *>(pData));
}



/*
std::ostream & operator<<(std::ostream &os, const MinVRDatum& p)
{
    return os << p.to_str();
}
*/
