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

MinVRDatum* CreateMinVRDatumDouble() { return new MinVRDatumDouble(MVRFLOAT,3.4,"ralph"); }
MinVRDatum* CreateMinVRDatumInt() { return new MinVRDatumInt(MVRINT,6,"henry"); }



/*
std::ostream & operator<<(std::ostream &os, const MinVRDatum& p)
{
    return os << p.to_str();
}
*/
