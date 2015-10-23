// -*-c++-*-
#ifndef MINVR_DATACOLLECTION_H
#define MINVR_DATACOLLECTION_H

#include <map>
#include <list>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include "Cxml/Cxml.h"
#include "VRDatum.h"
#include "VRDatumFactory.h"

// A collection of conveniences and useful functions for the data
// index and data queue objects.
class VRDataCollection {
protected:
  VRDatumFactory factory;

  // This is just a convenience to map strings to object type numbers.
  std::map<std::string, MVRTYPE_ID> mvrTypeMap;

public:
  VRDataCollection();

  std::string serialize(const std::string trimName, VRDatumPtr pdata);
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

  int deserializeInt(const char* valueString);
  double deserializeDouble(const char* valueString);
  std::string deserializeString(const char* valueString);
  MVRVecFloat deserializeVecFloat(const char* valueString);
  // Don't need a deserializeContainer. That happens in walkXML().

  /// Step 8 of the data type addition instructions in VRDatum.h is
  /// to add a specialized method here.  Also add a deserialize method
  /// above.
  virtual std::string addValue(const std::string valName, int value) = 0;
  virtual std::string addValue(const std::string valName, double value) = 0;
  virtual std::string addValue(const std::string valName, std::string value) = 0;
  virtual std::string addValue(const std::string valName, MVRVecFloat value) = 0;
  virtual std::string addValue(const std::string valName,
                               MVRContainer value) = 0;

};

#endif
