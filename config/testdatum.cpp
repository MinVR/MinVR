#include <map>
#include <iostream>
#include <stdexcept>
#include "MinVRDatum.h"
#include "MinVRDatumFactory.h"

int main() {
  // suppose this is the "singleton" instance for the MinVRDatumFactory
  // (this is an example! Singletons are not implemented like this!)
  MinVRDatumFactory *factory = new MinVRDatumFactory;
  factory->RegisterMinVRDatum(MVRFLOAT, CreateMinVRDatumDouble);
  factory->RegisterMinVRDatum(MVRINT, CreateMinVRDatumInt);

  int i = 14;
  double f = 3.1415926;
  MinVRDatumPtr s1 = factory->CreateMinVRDatum(MVRINT, &i);
  MinVRDatumPtr s2 = factory->CreateMinVRDatum(MVRFLOAT, &f);
  std::cout << s1.intVal()->getValue() << std::endl;
  std::cout << s2.doubleVal()->getValue() << std::endl;
  s2->doSomething();
  // will throw an error
  try {
    MinVRDatumPtr s3 = factory->CreateMinVRDatum(-1, &f);
    s3->doSomething();
  } catch(const std::exception& e) {
    std::cout<<"caught exception: "<<e.what()<<std::endl;
  }
  return 0;
}
