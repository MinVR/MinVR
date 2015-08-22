#include "MinVRDatum.h"
#include "MinVRDatumFactory.h"
#include "MinVRDataIndex.h"

// Step 5 of the specialization instructions (in MinVRDatum.h) is to
// add an entry here to register the new data type.
MinVRDataIndex::MinVRDataIndex() {
  factory.RegisterMinVRDatum(MVRFLOAT, CreateMinVRDatumDouble);
  factory.RegisterMinVRDatum(MVRINT, CreateMinVRDatumInt);
}

bool MinVRDataIndex::addValueInt(const std::string valName, int value) {

  MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRINT, &value);
  return mindex.insert(MinVRDataMap::value_type(valName, obj)).second;
}

bool MinVRDataIndex::addValueDouble(const std::string valName, double value) {

  MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRFLOAT, &value);
  //std::cout << "added " << obj.doubleVal()->getValue() << std::endl;
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

std::string MinVRDataIndex::getDescription(const std::string valName) {
  MinVRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {
    throw std::runtime_error(std::string("never heard of ") + valName);
  } else {
    return it->second->getDescription() + " " + valName + ";";
  }
}

std::string MinVRDataIndex::serialize(const std::string valName) {
  MinVRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {
    throw std::runtime_error(std::string("never heard of ") + valName);
  } else {
    return it->second->getDescription() + " " + valName + ";" +
      it->second->serialize();
  }
}


