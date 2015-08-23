#include <map>
#include <iostream>
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

  // This is just a convenience function to map strings to object type numbers.
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
  bool walkXML(element* node);

  bool processValue(const char* name,
                    const char* type,
                    const char* valueString);


  /// Step 6 of the data type addition instructions in MinVRDatum.h is
  /// to add a specialized method here.
  bool addValueInt(const std::string valName, int value);
  bool addValueDouble(const std::string valName, double value);

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
//   - Add a string data type to check on the add instructions.
//
//   - Add the struct data type, a collection of primitive data types,
//     or other structs.
//
//   - Support nested names in the index, with "default directory"
//     functionality.
//
//   - Should be ready to work with the XML parser at this point.
//
//   - Add the vector types, and whatever else.
