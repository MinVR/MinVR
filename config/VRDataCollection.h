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
  std::map<std::string, VRCORETYPE_ID> mVRTypeMap;

public:
  VRDataCollection();

  std::string serialize(const std::string trimName, VRDatumPtr pdata);
  virtual std::string serialize(const std::string valName) = 0;

  // Prints a vaguely tree-ish representation of an XML parse.  Just
  // an aid to debugging, really.
  bool printXML(element* node, std::string prefix);

  // Tries to guess a data type from the ASCII representation.
  VRCORETYPE_ID inferType(const std::string valueString);

  // Start from the root node of an XML document and process the
  // results into entries in the data index.
  bool walkXML(element* node, std::string nameSpace);
  // A functional part of the walkXML apparatus.
  bool processValue(const char* name,
                    VRCORETYPE_ID type,
                    const char* valueString);

  VRInt deserializeInt(const char* valueString);
  VRDouble deserializeDouble(const char* valueString);
  VRString deserializeString(const char* valueString);
  VRDoubleArray deserializeDoubleArray(const char* valueString);
  // Don't need a deserializeContainer. That happens in walkXML().

  /// Step 8 of the data type addition instructions in VRDatum.h is
  /// to add a specialized method here.  Also add a deserialize method
  /// above.
  virtual std::string addData(const std::string valName, VRInt value) = 0;
  virtual std::string addData(const std::string valName, VRDouble value) = 0;
  virtual std::string addData(const std::string valName, VRString value) = 0;
  virtual std::string addData(const std::string valName, VRDoubleArray value) = 0;
  virtual std::string addData(const std::string valName,
                              VRContainer value) = 0;

};

#endif
