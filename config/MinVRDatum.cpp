#include "MinVRDatum.h"

MinVRDatumInt::MinVRDatumInt(const int inVal,
                             const std::string inName) :
  MinVRDatum(MVRINT, inName), value(inVal) {
  description = "int " + inName + ";" ;
};



std::string MinVRDatumInt::serialize() {
  return std::string("hello");
}

void MinVRDatumInt::doSomething() const {
  std::cout << "Something with an int called: "<< name << " (" << ")" << std::endl;
}

MinVRDatum* CreateMinVRDatumInt(void *pData, std::string name) {
  return new MinVRDatumInt(*static_cast<int *>(pData),
                           name);
}

////////////////////////////////////////////

MinVRDatumDouble::MinVRDatumDouble(const double inVal,
                                   const std::string inName) :
  MinVRDatum(MVRFLOAT, inName), value(inVal) {
  description = "float " + inName + ";" ;
};


std::string MinVRDatumDouble::serialize() {
  return std::string("good-bye");
}

void MinVRDatumDouble::doSomething() const {
  std::cout << "Something with a double called: " << name << " (" << ")" << std::endl;
}

MinVRDatum* CreateMinVRDatumDouble(void *pData, std::string name) {
  return new MinVRDatumDouble(*static_cast<double *>(pData),
                              name);
}



/*
std::ostream & operator<<(std::ostream &os, const MinVRDatum& p)
{
    return os << p.to_str();
}
*/
