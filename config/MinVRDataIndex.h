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
  virtual bool addValueInt(const std::string valName, int value) = 0;
  virtual bool addValueDouble(const std::string valName, double value) = 0;
  virtual bool addValueString(const std::string valName, std::string value) = 0;
  virtual bool addValueContainer(const std::string valName,
                                 MVRContainer value) = 0;

};

// This object maintains an index, a collection of names and pointers
// to MinVRDatum objects, which can be used to simulate a dynamically
// typed computing environment in C++, a strongly-typed system.  In
// this system, values have types, not variables.  All variables, of
// whatever type, are equivalent.  There is a container type, which
// can include an arbitrary collection of any other variables.
//
// The precise collection of types currently implemented is somewhat
// random, but the MinVRDatum.h file contains instructions for
// extending the system to any C++ type, including the STL types,
// whatever, go wild.  It supports the concept of a 'namespace' and
// scoping of its value names, so names within a container type can
// override names higher in the tree hierarchy.  See the getName()
// function for a little more explanation.
//
// Data values can be easily serialized for transport over a network,
// and deserialized, for receiving them, and interning the new values
// into memory.  The system supports an XML format for transmission
// and storage.
//
// Tom Sgouros 8/29/2015
//
// To use:
//
// Create an index object, and add values to the store in one of three ways.
//
//  1. You can add data with the specialized add functions, such as
//     addValueInt(), addValueFloat(), etc.  These take a name and a
//     value and park them in the index.
//
//  2. You can feed some serialized data to one of the addValue()
//     functions.  This is just string data, that looks something like
//     this:
//
//     <flora type="int">42</flora>
//
//     <bob type="container">
//        <flora type="int">42</flora>
//        <pi type="float">3.1415926</pi>
//     </bob>
//
//     The type attributes are usually superfluous, since the code can
//     usually infer the type from the value string.  However, future
//     implemented data types might make the inference step less
//     reliable, so bear that in mind.
//
//  3. Feed a file containing XML into processXMLFile().
//
//  Once an index has entries, they can be retrieved at your pleasure
//  with getDatum() or serialize().  The getDatum() method returns a
//  pointer to a MinVRDatum object, so can be used directly in your
//  program.  I hate remembering how to spell the static_cast<>()
//  options, so these are provided as a convenience via a helper class
//  methods to the pointer objects.  So (int)p->getDatum() gets you an
//  integer and (double)p->getDatum() gets you a double, so long as the
//  relevant objects actually contain an integer and double.
//
//  The data values in this system are meant to be immutable.
//  However, they mostly just seem that way.  There are setter methods
//  for them, used mainly as an optimization.  That is, rather than
//  trash an object with the old value and create a new object with
//  the new value, we just swap values.
//
//  This file should come with an exercz.cpp that does an ok job of
//  illustrating some of the usage.  You can also use that program to
//  learn about the namespace and name scoping concepts.  It's sort of
//  like an interpreter for some primitive language environment whose
//  only functions are assigning variables and reading them.
//
//  The system encompasses the MinVRDataIndex class, whose job it is
//  to track a set of names and corresponding MinVRDatumPtr objects,
//  smart pointers to MinVRDatum objects, that hold a type and value.
//  There is also a MinVRDatumFactory object, whose only use is
//  creating the specializations of the MinVRDatum object for each of
//  the supported data types.  It's all meant to be as portable as
//  possible and not rely on any external libraries.  It uses an XML
//  reader, see those files for the original credit.
//
class MinVRDataIndex : MinVRDataCollection {
private:

  typedef std::map<std::string, MinVRDatumPtr> MinVRDataMap;
  MinVRDataMap mindex;

  // If this is 1, new values will overwrite old ones.  For -1, new
  // values will just bounce off.  And zero will cause an exception if
  // an overwrite is attempted.  Except containers, who are always
  // happy to receive new values and add to their existing list.  The
  // default class constructor allows overwrites.
  int overwrite;

public:
    MinVRDataIndex() : overwrite(1) {};

  void setOverwrite(const int inVal) { overwrite = inVal; }

  // Finds a name, given a namespace. Note that the name might be in a
  // senior namespace to the one specified.  That is, if you have a
  // value called flora, that exists inside a container called cora,
  // but also in a subsidiary container called nora, then, well,
  // here's the example:
  //
  // /cora/flora = 6
  // /cora/nora/flora = 7
  //
  // If the namespace is /cora, the value of flora is 6, but if the
  // namespace is /cora/nora, flora is 7.
  std::string getName(const std::string valName,
                      const std::string nameSpace);

  // Returns a pointer to the value with a given name (and namespace)
  MinVRDatumPtr getDatum(const std::string valName);
  MinVRDatumPtr getDatum(const std::string valName,
                         const std::string nameSpace);

  // The getValue function relies on the helper function defined with
  // MinVRDatum.  Read about it there, but it is just a cute trick to
  // allow programmers to access values directly from the index.  So
  // access your values like this:
  //
  //   int p = index->getDatum(name, nameSpace)->getValue()
  //
  // or like this:
  //
  //   int p = index->getValue(name, nameSpace)
  //
  // It's your choice; the two are identical in overhead.  If you're
  // really worried about overhead, you can do this instead:
  //
  //   int p = index->getValue(name, nameSpace)->getValueInt()
  //
  // This version does a kind of type checking, but also brings you
  // the specialized MinVRDatum object which you might want for some
  // other nefarious reason.
  //
  //   int p = index->getValue(name, nameSpace).intVal()->getValueInt()
  //
  MinVRDatumHelper<MinVRDatum> getValue(const std::string valName) {
    return getDatum(valName)->getValue();
  }
  MinVRDatumHelper<MinVRDatum> getValue(const std::string valName,
                                        const std::string nameSpace) {
    return getDatum(valName, nameSpace)->getValue();
  }

  // The description of an index entry describes only the name and
  // type, not the value.
  std::string getDescription(const std::string valName);
  std::string getDescription(const std::string valName,
                             const std::string nameSpace);

  // This is the name, type, value, expressed as an XML fragment.
  using MinVRDataCollection::serialize;
  std::string serialize(const std::string valName);
  std::string serialize(const std::string valName,
                        const std::string nameSpace);

  // Takes a serialized bit of data and incorporates it into the data
  // index.
  bool addValue(const std::string serializedData);
  bool addValue(const std::string serializedData,
                const std::string nameSpace);

  // Process the contents of a given XML file into the index.
  bool processXMLFile(std::string fileName);

  // Returns a list of all the names in the map.  Note this really is
  // a list of strings, not a MVRContainer.
  std::list<std::string> getDataNames();

  // These are specialized set methods.  They seem a little unhip, but
  // it's because I find this easier than remembering how to spell the
  // pointers and casts.

  /// Step 6 of the data type addition instructions in MinVRDatum.h is
  /// to add a specialized method here.
  bool addValueInt(const std::string valName, int value);
  bool addValueDouble(const std::string valName, double value);
  bool addValueString(const std::string valName, std::string value);
  bool addValueContainer(const std::string valName, MVRContainer value);

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
//     print its value and serialization, 'p' for print everything,
//     etc. [DONE]
//
//   - 'c name' for adopting a namespace, 'a string' for add a
//     variable to store, etc. [DONE]
//
//   - Need to support name conflicts by either overwriting the new
//     value or throwing an exception or some other way of defending
//     the old value.  User should be able to choose. [DONE]
//
//   - Support nested names in the index, with "default directory"
//     functionality.
//
//        - Need a getName(valName, nameSpace) [DONE]
//        - Leave getValue, getDescription, and serialize as they were,
//          needing a fully-qualified name as input. [DONE]
//        - They can all have second forms that have a nameSpace, but
//          call the original under the covers. [DONE]
//
//     Needs to handle blank namespaces, and ignore the namespace for names
//     that begin with a slash. [DONE]
//
//   - Should be ready to work with the XML parser for entire files at
//     this point. [DONE]
//
//   - Add the vector types, and whatever else.  Boolean?
//
//   - Make the parser infer data types where possible, rather than relying
//     on the type attribute. [DONE]
//
//   - Add container typedef to MinVRDatum [DONE]
//
//   - Add helper class to MinVRDatum, implement getValue() [DONE]
//
//    - Do something about MinVRDatum setValue() methods? Maybe
//      comparable to the virtual getValue() methods? [NO, Users should not use]
//
//   - change MinVRDataIndex::getValue() to getDatum() [DONE]
//
//   - Can we add a helper class to data index? [DONE]
