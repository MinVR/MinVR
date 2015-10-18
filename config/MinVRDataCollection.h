#include <map>
#include <list>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include "Cxml.h"
#include "element.h"
#include "MinVRDatum.h"
#include "MinVRDatumFactory.h"

// A collection of conveniences and useful functions for the data
// index and data queue objects.
class MinVRDataCollection {
protected:
  MinVRDatumFactory factory;

  // This is just a convenience to map strings to object type numbers.
  std::map<std::string, MVRTYPE_ID> mvrTypeMap;

public:
  MinVRDataCollection();

  std::string serialize(const std::string trimName, MinVRDatumPtr pdata);
  virtual std::string serialize(const std::string valName) = 0;

  // Prints a vaguely tree-ish representation of an XML parse.  Just
  // an aid to debugging, really.
  bool printXML(element* node, std::string prefix);

  // Tries to guess a data type from the ASCII representation.
  MVRTYPE_ID inferType(const std::string valueString);

  // Start from the root node of an XML document and process the
  // results into entries in the data index.
  bool walkXML(element* node, std::string nameSpace);
  // A functional part of the walkXML apparatus.
  bool processValue(const char* name,
                    MVRTYPE_ID type,
                    const char* valueString);

  /// Step 6 of the data type addition instructions in MinVRDatum.h is
  /// to add a specialized method here.
  virtual bool addValue(const std::string valName, int value) = 0;
  virtual bool addValue(const std::string valName, double value) = 0;
  virtual bool addValue(const std::string valName, std::string value) = 0;
  virtual bool addValue(const std::string valName, MVRVecFloat value) = 0;
  virtual bool addValue(const std::string valName,
                        MVRContainer value) = 0;

};

