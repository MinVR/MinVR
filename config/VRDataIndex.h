// -*-c++-*-
#ifndef MINVR_DATAINDEX_H
#define MINVR_DATAINDEX_H
#include "VRDataCollection.h"

// This object maintains an index, a collection of names and pointers
// to VRDatum objects, which can be used to simulate a dynamically
// typed computing environment in C++, a strongly-typed system.  In
// this system, values have types, not variables.  All variables, of
// whatever type, are equivalent.  There is a container type, which
// can include an arbitrary collection of any other variables.
//
// It supports the concept of a 'namespace' and
// scoping of its value names, so names within a container type can
// override names higher in the tree hierarchy.  See the getName()
// function for a little more explanation.
//
// Data values can be easily serialized for transport over a network,
// and deserialized, for receiving them, and interning the new values
// into memory.  The system supports an XML format for transmission
// and storage.
//
// The XML parser used here is based on the "Simple C++ XML Parser"
// from the CodeProject article by "BratilaRazvan", 2010.
// see: http://www.codeproject.com/Articles/111243/Simple-Cplusplus-XML-Parser
//
// Tom Sgouros 8/29/2015
//
// p.s. The precise collection of types currently implemented is
// somewhat random, but the VRDatum.h file contains instructions for
// extending the system to any C++ type, including the STL types,
// whatever, go wild.  This may not be important to actually using the
// system, read on for those instructions.
//
// To use:
//
// Create an index object, and add values to the store in one of three ways.
//
//  1. You can add data with the specialized add functions, such as
//     addValueInt(), addValueDouble(), etc.  These take a name and a
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
//        <pi type="double">3.1415926</pi>
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
//  with getValue() and serialize().  The getValue() method returns
//  a pointer an object that can be used directly in your program.  I
//  hate remembering how to spell the static_cast<>() options, so
//  these are provided as a convenience via a helper class methods to
//  the pointer objects.  So (int)p->getValue() gets you an integer
//  and (double)p->getValue() gets you a double. (So long as the
//  relevant objects actually contain an integer and double, otherwise
//  you get an error.)
//
//  To use, do this:
//
//    VRDataIndex *index = new VRDataIndex;
//    int a = 4;
//    index->addValue(std::string("/george", a);
//
//  This incorporates an integer value of 4 into the index with the
//  name "george" that appears at the root (global) level.
//
//  When you want to refer to the value you put in, do this:
//
//    int p = index->getValue("/george");
//
//  or
//
//    int p = index->getValue("george", "/");
//
//  Where the second argument is the "namespace", which is arranged
//  vaguely like a directory structure, with slashes (/) separating
//  the names.  You can also do this:
//
//    std::string s = index->serialize("/george")
//
//    VRDataIndex *newIndex = new VRDataIndex;
//    newIndex->addSerializedValue(s);
//    int b = newIndex->getValue("/george");
//
//  The b variable now comtains the same value as a, with the same
//  name.
//
//
//  This file should come with an exercz.cpp that does an ok job of
//  illustrating some of the usage.  You can also use that program to
//  learn about the namespace and name scoping concepts.  It's sort of
//  like an interpreter for some primitive language environment whose
//  only functions are assigning variables and reading them.
//
//  The system encompasses the VRDataIndex class, whose job it is
//  to track a set of names and corresponding VRDatumPtr objects,
//  smart pointers to VRDatum objects, that hold a type and value.
//  There is also a VRDatumFactory object, whose only use is
//  creating the specializations of the VRDatum object for each of
//  the supported data types.  It's all meant to be as portable as
//  possible and not rely on any external libraries.  It uses an XML
//  reader, see those files for the original credit.
//
class VRDataIndex : VRDataCollection {
private:

  typedef std::map<std::string, VRDatumPtr> VRDataMap;
  VRDataMap mindex;

  // If this is 1, new values will overwrite old ones.  For -1, new
  // values will just bounce off.  And zero will cause an exception if
  // an overwrite is attempted.  Except containers, who are always
  // happy to receive new values and add to their existing list.  The
  // default class constructor allows overwrites.
  int overwrite;

public:
  VRDataIndex() : overwrite(1) {};

  void setOverwrite(const int inVal) { overwrite = inVal; }

  // Finds an entry in the data index, given a name and
  // namespace. Note that the name might be in a senior namespace to
  // the one specified.  That is, if you have a value called flora,
  // that exists inside a container called cora, but also in a
  // subsidiary container called nora, then, well, here's the example:
  //
  // /cora/flora = 6
  // /cora/nora/flora = 7
  //
  // If the namespace is /cora, the value of flora is 6, but if the
  // namespace is /cora/nora, flora is 7.

  // Returns an iterator pointing to an entry in the DataMap.  The
  // return value is a pair<string, VRDatumPtr>, so it->first is the
  // name and it->second is the datum object.
  VRDataMap::iterator getEntry(const std::string valName,
                                     const std::string nameSpace);
  VRDataMap::iterator getEntry(const std::string valName);

  // Returns the fully qualified name of the value.
  std::string getName(const std::string valName,
                      const std::string nameSpace);

  // Returns a pointer to the value with a given name (and namespace)
  VRDatumPtr getDatum(const std::string valName);
  VRDatumPtr getDatum(const std::string valName,
                      const std::string nameSpace);

  // Just a utility to return the tail end of the fully qualified name.
  // i.e. trimName("cora/flora", "/bob/nora") is "flora".  This does not
  // look in the index at all, just manipulates strings.
  std::string getTrimName(const std::string valName,
                          const std::string nameSpace);
  std::string getTrimName(const std::string valName);

  // Another utility, meant to pull a name apart on the slashes.
  std::vector<std::string> explodeName(const std::string fullName);

  // Still another utility, for safety's sake.
  std::string validateNameSpace(const std::string nameSpace);

  // The getValue function relies on the helper function defined with
  // VRDatum.  Read about it there, but it is just a cute trick to
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
  // the specialized VRDatum object which you might want for some
  // other nefarious reason.
  //
  //   int p = index->getValue(name, nameSpace).intVal()->getValueInt()
  //
  VRDatumHelper<VRDatum> getValue(const std::string valName) {
    return getDatum(valName)->getValue();
  }
  VRDatumHelper<VRDatum> getValue(const std::string valName,
                                  const std::string nameSpace) {
    return getDatum(valName, nameSpace)->getValue();
  }

  // The description of an index entry describes only the name and
  // type, not the value.
  std::string getDescription(const std::string valName);
  std::string getDescription(const std::string valName,
                             const std::string nameSpace);

  // This is the name, type, value, expressed as an XML fragment.
  using VRDataCollection::serialize;
  std::string serialize(const std::string valName);
  std::string serialize(const std::string valName,
                        const std::string nameSpace);

  // Takes a serialized bit of data and incorporates it into the data
  // index.
  bool addSerializedValue(const std::string serializedData);
  bool addSerializedValue(const std::string serializedData,
                          const std::string nameSpace);

  // Process the contents of a given XML file into the index.
  bool processXMLFile(std::string fileName);

  // Returns a list of all the names in the map.  Note this really is
  // a list of strings, not a MVRContainer.  (No difference, really,
  // but we want to keep them semantically separate.)
  std::list<std::string> getDataNames();
  std::list<std::string> getDataNames(const std::string containerName);

  // These are specialized set methods.  They seem a little unhip, but
  // it's because I find this easier than remembering how to spell the
  // pointers and casts.

  /// Step 6 of the data type addition instructions in VRDatum.h is
  /// to add a specialized method here.
  std::string addValue(const std::string valName, int value);
  std::string addValue(const std::string valName, double value);
  std::string addValue(const std::string valName, std::string value);
  std::string addValue(const std::string valName, MVRArrayDouble value);

  // There is a semantic difference between addValue() for a primitive
  // value and addValue() for a container.  One creates an object of
  // the name and value given.  The second (for containers) adds the
  // contents of the given container value to the container of the
  // given name.
  std::string addValue(const std::string valName, MVRContainer value);
  // Adds an empty container.
  std::string addValue(const std::string valName);

  void rmValue(const std::string valName, const std::string nameSpace);
  void rmValue(const std::string valName);

  //  std::string replaceValue(const std::string containerName,
  //                         const MVRContainer newContents);

  // Mostly just for debug purposes.
  void printWholeKitAndKaboodle();

};


// Where are we going with this: We have an index that contains
// pointers to more-or-less arbitrary data types, yay.  On the to-do
// list:
//
//   - Write the serialize method, at the VRDatum level, where
//     it only encodes the value, and at the VRDataIndex level,
//     where you add the type description (also from VRDatum),
//     the name (from the index) and the value. [DONE]
//
//   - VRDatum constructors that work from the serialized form
//        int george 6
//         ^    ^    ^
//         |    |    |
//         |    |    handled in VRDatum
//         |    handled in VRDataIndex
//         handled in VRDataIndex (choice of Datum specialization) [DONE]
//
//   - Add the interpreters to VRDataIndex to read the whole
//     serialized form and invoke the appropriate VRDatum
//     constructor. [DONE]
//
//   - Add a string data type to check on the add instructions. [DONE]
//
//   - Add the struct data type, a collection of primitive data types,
//     or other structs.  The data inside the VRDatum for this
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
//        vector<double> [DONE]
//        matrix<double> [TBD]
//
//   - Make the parser infer data types where possible, rather than relying
//     on the type attribute. [DONE]
//
//   - Add container typedef to VRDatum [DONE]
//
//   - Add helper class to VRDatum, implement getValue() [DONE]
//
//    - Do something about VRDatum setValue() methods? Maybe
//      comparable to the virtual getValue() methods? [NO, Users should not use]
//
//   - change VRDataIndex::getValue() to getDatum() [DONE]
//
//   - Can we add a helper class to data index? [DONE]
//
//   - Need a human-readable and a machine-readable (i.e. quicker)
//     format for serialization. [TBD]
//
//   - The mvrTypeMap is clunky and is not attached to the VRDatum
//     description field.  The mapping between type ID and description
//     should appear only once, somewhere. [DONE]
//
//   - Improve (and test) adding values to containers.  Values should be
//     added as VRDatum objects, or just a 'mv' command that moves an
//     existing object from one namespace in the index to another.
//
//   Need to simplify the process of adding data types.
//
//   - Make CreateVRDatum functions into templates?
//
//   - Make VRDatumPtr::intVal(), etc, into templates.
//
//   - After those are done, the VRDataIndex::addValue() methods can
//     probably be template-ized, too.
//
//   -
#endif
