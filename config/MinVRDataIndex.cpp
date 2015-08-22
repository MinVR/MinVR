#include "MinVRDatum.h"
#include "MinVRDatumFactory.h"
#include "MinVRDataIndex.h"

MinVRDataIndex::MinVRDataIndex() {
  factory.RegisterMinVRDatum(MVRFLOAT, CreateMinVRDatumDouble);
  factory.RegisterMinVRDatum(MVRINT, CreateMinVRDatumInt);
}

bool MinVRDataIndex::addValueInt(const std::string valName, int value) {
  std::cout << "adding: " << valName << ": " << value << std::endl;

  MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRINT, &value);
  std::cout << "added " << obj.intVal()->getValue() << std::endl;
  return mindex.insert(MinVRDataMap::value_type(valName, obj)).second;
}

bool MinVRDataIndex::addValueDouble(const std::string valName, double value) {
  std::cout << "adding: " << valName << ": " << value << std::endl;

  MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRFLOAT, &value);
  std::cout << "added " << obj.doubleVal()->getValue() << std::endl;
  return mindex.insert(MinVRDataMap::value_type(valName, obj)).second;
}

MinVRDatumPtr MinVRDataIndex::getValue(const std::string valName) {
  MinVRDataMap::const_iterator it = mindex.find(valName);
  if (it == mindex.end()) {
    throw std::runtime_error(std::string("never heard of ") + valName);
  } else {
    return it->second;
  }
}

