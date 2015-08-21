#include "MinVRDatum.h"

std::string MinVRDatumInt::serialize() {
  return std::string("hello");
}

std::string MinVRDatumInt::getDescription() {
  return std::string("int");
}

void MinVRDatumInt::doSomething() const {
  std::cout << "Something with an int called: "<< name << std::endl;
}

////////////////////////////////////////////

std::string MinVRDatumDouble::serialize() {
  return std::string("good-bye");
}

std::string MinVRDatumDouble::getDescription() {
  return std::string("float");
}

void MinVRDatumDouble::doSomething() const {
  std::cout << "Something with a double called: " << name << std::endl;
}

MinVRDatum* CreateMinVRDatumInt(void *pData, std::string name) {
  return new MinVRDatumInt(MVRINT,
                           *static_cast<int *>(pData),
                           name);
}

MinVRDatum* CreateMinVRDatumDouble(void *pData, std::string name) {
  return new MinVRDatumDouble(MVRFLOAT,
                              *static_cast<double *>(pData),
                              name);
}



/*
std::ostream & operator<<(std::ostream &os, const MinVRDatum& p)
{
    return os << p.to_str();
}
*/
