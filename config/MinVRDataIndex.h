#include <map>
#include <iostream>
#include <stdexcept>
#include <tr1/memory>
#include "MinVRDatum.h"
#include "MinVRDatumFactory.h"

// This object maintains an index, a collection of names and
// MinVRDatum objects.
class MinVRDataIndex {
private:

  typedef std::map<std::string, MinVRDatumPtr> MinVRDataMap;
  MinVRDataMap mindex;
  MinVRDatumFactory factory;

public:
  MinVRDataIndex ();

  MinVRDatumPtr getValue(const std::string valName);

  bool addValueInt(const std::string valName, int value);
  bool addValueDouble(const std::string valName, double value);

};
