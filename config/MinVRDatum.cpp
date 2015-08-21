#include "MinVRDatum.h"

std::string MinVRDatumInt::serialize() {
  return std::string("hello");
}

std::string MinVRDatumInt::getDescription() {
  return std::string("int");
}

void MinVRDatumInt::doSomething() const {
    std::cout<<"Something with an int"<<std::endl;
}

////////////////////////////////////////////

std::string MinVRDatumDouble::serialize() {
  return std::string("good-bye");
}

std::string MinVRDatumDouble::getDescription() {
  return std::string("float");
}

void MinVRDatumDouble::doSomething() const {
  std::cout<<"Something with a double"<<std::endl;
}

MinVRDatum* CreateMinVRDatumInt(void *pData) {
  return new MinVRDatumInt(MVRINT,
                           *static_cast<int *>(pData),
                           "henry");
}

MinVRDatum* CreateMinVRDatumDouble(void *pData) {
  return new MinVRDatumDouble(MVRFLOAT,
                              *static_cast<double *>(pData),
                              "ralph");
}



/*
std::ostream & operator<<(std::ostream &os, const MinVRDatum& p)
{
    return os << p.to_str();
}
*/
