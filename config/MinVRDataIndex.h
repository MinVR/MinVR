#include <map>
#include <list>
#include <iostream>
#include <string>
#include <stdexcept>
#include "Cxml.h"
#include "element.h"
#include "MinVRDatum.h"
#include "MinVRDatumFactory.h"

// This object maintains an index, a collection of names and
// MinVRDatum objects.
class MinVRDataIndex {
private:

  typedef std::map<std::string, MinVRDatumPtr> MinVRDataMap;
  MinVRDataMap mindex;
  MinVRDatumFactory factory;

  // This is just a convenience to map strings to object type numbers.
  std::map<std::string, MVRTYPE_ID> mvrTypeMap;

public:
  MinVRDataIndex ();

  MinVRDatumPtr getValue(const std::string valName);
  std::string getDescription(const std::string valName);
  std::string serialize(const std::string valName);

  bool addValue(const std::string serializedData);
  bool addValue(const std::string serializedData,
                const std::string nameSpace);

  bool printXML(element* node, int level);

  // Start from the root node of an XML document and process the
  // results into entries in the data index.
  bool walkXML(element* node, std::string nameSpace);
  // A functional part of the walkXML apparatus.
  bool processValue(const char* name,
                    const char* type,
                    const char* valueString);
  // Tries to guess a data type from the ASCII representation.
  //MVRTYPE_ID inferType(const char* valueString);

  // Process the contents of a given XML file into the index.
  bool processXMLFile(std::string fileName);

  // Returns a list of all the names in the map.
  std::list<std::string> getDataNames();

  /// Step 6 of the data type addition instructions in MinVRDatum.h is
  /// to add a specialized method here.
  bool addValueInt(const std::string valName, int value);
  bool addValueDouble(const std::string valName, double value);
  bool addValueString(const std::string valName, std::string value);
  bool addValueContainer(const std::string valName, std::list<std::string> value);

};


// Where are we going with this: We have an index that contains
// pointers to more-or-less arbitrary data types, yay.  On the to-do
// list:
//
//   - Write the serialize method, at the MinVRDatum level, where
//     it only encodes the value, and at the MinVRDataIndex level,
//     where you add the type description (also from MinVRDatum),
//     the name (from the index) and the value. [DONE]
//
//   - MinVRDatum constructors that work from the serialized form
//        int george 6
//         ^    ^    ^
//         |    |    |
//         |    |    handled in MinVRDatum
//         |    handled in MinVRDataIndex
//         handled in MinVRDataIndex (choice of Datum specialization) [DONE]
//
//   - Add the interpreters to MinVRDataIndex to read the whole
//     serialized form and invoke the appropriate MinVRDatum
//     constructor. [DONE]
//
//   - Add a string data type to check on the add instructions. [DONE]
//
//   - Add the struct data type, a collection of primitive data types,
//     or other structs.  The data inside the MinVRDatum for this
//     container type is just going to be a vector of map iterators,
//     pointing at map entries in the main index. (I think) [DONE]
//
//   - Make testindex into an examination tool.  'p variable' for
//     print its value and serialization, 'c name' for adopting a
//     namespace, 'p' for print everything, 'a string' for add a
//     variable to store, etc.
//
//   - Support nested names in the index, with "default directory"
//     functionality.
//
//   - Should be ready to work with the XML parser for entire files at
//     this point.
//
//   - Add the vector types, and whatever else.  Make the parser infer
//     data types where possible, rather than relying on the type
//     attribute.
