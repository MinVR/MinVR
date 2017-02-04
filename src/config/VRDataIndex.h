// -*-c++-*-
/* ================================================================================

This file is part of the MinVR Open Source Project.

File: extend/PluginInterface.h

Original Author(s) of this File:
  Tom Sgouros 8/29/2015

Author(s) of Significant Updates/Modifications to the File:
	...

-----------------------------------------------------------------------------------
*/
#ifndef MINVR_DATAINDEX_H
#define MINVR_DATAINDEX_H

#include "VRDatumFactory.h"
#include "Cxml/Cxml.h"
/// The VRDataIndex object maintains an index, a collection of names
/// and pointers to VRDatum objects, which can be used to create a
/// dynamically typed computing environment in C++, a strongly-typed
/// system.  In this system, values have types, variables do not
/// (though they contain values that do).  All variables, of whatever
/// type, are equivalent.  There is a container type, which can
/// include an arbitrary collection of any other variables.
///
/// It supports the concept of a 'namespace' and scoping of its value
/// names, so names within a container type can override names higher
/// in the tree hierarchy.
///
/// Data values can be easily serialized for transport over a network,
/// and deserialized, for receiving them, and interning the new values
/// into memory.  The system supports an XML format for transmission
/// and storage.
///
/// The system state can be preserved by pushing it onto a stack, and
/// later restored by popping that stack.
///
/// The XML parser used here is based on the "Simple C++ XML Parser"
/// from the CodeProject article by "BratilaRazvan", 2010.
/// see: http:///www.codeproject.com/Articles/111243/Simple-Cplusplus-XML-Parser
/// (It does not support entities, or CDATA declarations.)
///
///
/// To use the system, it is useful to understand the concept of a
/// namespace.  If you don't want to bother, just remember this: a
/// namespace is just a container's name, with a '/' after it.  The
/// rest is just detail.
///
/// Containers define a namespace as well as hold a bunch of objects
/// inside them.  The two are very closely related, but are
/// conceptually distinct.  From a user's point of view, a namespace is
/// a string that starts and ends with a '/' and may or may not have
/// some text in the middle.  Here are some possible namespaces:
///
///       /perry/
///       /freida/mary/
///       /     <- the root namespace to which all VRDataIndex objects belong.
///       /homer/henry/martha/
///
/// A container object is simply an object that "contains" other
/// values.  Those objects can be primitive VRDatum objects, or they
/// can also be containers themselves.  The container object contains a
/// list of the (relative) names of the objects it contains.  Here are
/// some possible names of containers:
///
///       /peter   which might contain  harold
///                                     norma
///                                and  flora
///
/// Note that the container lists the relative name.  When the
/// container is serialized, the serialized version contains only the
/// relative names.
///
///
///
/// The precise collection of types currently implemented is
/// somewhat random, but the VRDatum.h file contains instructions for
/// extending the system to any C++ type, including the STL types,
/// whatever, go wild.  This may not be important to actually using the
/// system, read on for those instructions.
///
///
/// To use:
///
/// Create an index object, and add values to the store in one of three ways.
///
///  1. You can add data with the specialized addData() functions.
///     These take a name and a value and park them in the index.  The
///     addData() with a single argument adds a container and you can
///     subsequently add items to it.
///
///  2. You can feed some serialized data to the addSerializedValue()
///     functions.  This is just string data, that looks something like
///     this:
///
///     <flora type="int">42</flora>
///
///     <bob type="container">
///        <flora type="int">42</flora>
///        <pi type="float">3.1415926</pi>
///     </bob>
///
///     The type attributes are usually superfluous, since the code can
///     usually infer the type from the value string.  However, future
///     implemented data types might make the inference step less
///     reliable, so bear that in mind.
///
///     The VRDatum objects preserve attribute information, so you can
///     assign attributes to your data values.  The code uses a
///     "separator" attribute for serializing/deserializing array
///     values, but is otherwise agnostic about attributes.  The
///     separator attribute works like this:
///
///     <cora type="intarray">3,4,5,6</cora>
///     <nora type="stringarray" separator="@">A string@B sting@C sing@D sin@E in@F i</nora>
///
///     If no separator is specified, the code uses ','.  You can use
///     backslash (\) to include a separator character in one of the
///     strings.  So:
///
///     <cora>Alpha,Beta,Gamma\,Delta,Epsilon</cora>
///
///     Comes out with four entries in the array, "Alpha", "Beta",
///     "Gamma,Delta", and "Epsilon".  See indextest.cpp for an example.
///
///  3. Feed a file containing XML into processXMLFile().  There is no
///     validation of the XML done, and there are some ways in which
///     this is not an arbitrary XML parser.  For example, the parser
///     assumes uniqueness of names within an element.  Two elements
///     of the same name within some parent element will overwrite one
///     another.  Validation of the XML is left to some other process,
///     hopefully with a schema that enforces this limitation.
///
///  Once an index has entries, they can be retrieved at your pleasure
///  with getValue() and serialize().  The getValue() method returns
///  a pointer an object that can be used directly in your program.  I
///  hate remembering how to spell the static_cast<>() options, so
///  these are provided as a convenience via a helper class methods to
///  the pointer objects.  So (int)p->getValue() gets you an integer
///  and (float)p->getValue() gets you a float. (So long as the
///  relevant objects actually contain an integer and float, otherwise
///  you get an error.)
///
///  To use, do this:
///
///    VRDataIndex *index = new VRDataIndex;
///    int a = 4;
///    index->addData("/george", a);
///
///  This incorporates an integer value of 4 into the index with the
///  name "george" that appears at the root (global) level.
///
///  When you want to refer to the value you put in, do this:
///
///    int p = index->getValue("/george");
///
///  or
///
///    int p = index->getValue("george", "/");
///
///  Where the second argument is the "namespace", which is arranged
///  vaguely like a directory structure, with slashes (/) separating
///  the names.  You can also do this:
///
///    std::string s = index->serialize("/george")
///
///    VRDataIndex *newIndex = new VRDataIndex;
///    newIndex->addSerializedValue(s);
///    int b = newIndex->getValue("/george");
///
///  The b variable now contains the same value as a, with the same
///  name.
///
///
///  This file should come with an exercz.cpp that does an ok job of
///  illustrating some of the usage.  You can also use that program to
///  learn about the namespace and name scoping concepts.  It's sort of
///  like an interpreter for some primitive language environment whose
///  only functions are assigning variables and reading them.
///
///  The system encompasses the VRDataIndex class, whose job it is
///  to track a set of names and corresponding VRDatumPtr objects,
///  smart pointers to VRDatum objects, that hold a type and value.
///  There is also a VRDatumFactory object, whose only use is
///  creating the specializations of the VRDatum object for each of
///  the supported data types.  It's all meant to be as portable as
///  possible and not rely on any external libraries.  It uses an XML
///  reader, see those files for the original credit.
///

namespace MinVR {

class VRDataIndex {
private:
  VRDatumFactory factory;

  // This is just a convenience to map strings to object type numbers.
  std::map<std::string, VRCORETYPE_ID> mVRTypeMap;

  typedef std::map<std::string, VRDatumPtr> VRDataMap;
  // Aspirational:
  //typedef std::map<std::string, std::vector<VRDatumPtr> > VRDataMap;
  VRDataMap mindex;

  // If this is 1, new values will overwrite old ones.  For -1, new
  // values will just bounce off.  And zero will cause an exception if
  // an overwrite is attempted.  Except containers, who are always
  // happy to receive new values and add to their existing list.  The
  // default class constructor allows overwrites.
  int overwrite;

  // Just a utility to return the tail end of the fully qualified name.
  // i.e. trimName("cora/flora", "/bob/nora") is "flora".  This does not
  // look in the index at all, just manipulates strings.
  std::string getTrimName(const std::string valName,
                          const std::string nameSpace);
  std::string getTrimName(const std::string valName);

  // Another utility, meant to pull a name apart on the slashes.
  std::vector<std::string> explodeName(const std::string fullName);

  // Returns the namespace, derived from a long, fully-qualified, name.
  std::string getNameSpace(const std::string fullName);

  // Tries to guess a data type from the ASCII representation.
  VRCORETYPE_ID inferType(const std::string valueString);

  // Start from the root node of an XML document and process the
  // results into entries in the data index.
  std::string walkXML(element* node, std::string nameSpace);
  // A functional part of the walkXML apparatus.
  std::string processValue(const std::string name,
                           VRCORETYPE_ID type,
                           const char* valueString,
                           const char separator);

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

  // These functions read an XML-encoded string and produce the value
  // implied.  There is no deserializeContainer, since that's what
  // walkXML does.
  VRInt deserializeInt(const char* valueString);
  VRFloat deserializeFloat(const char* valueString);
  VRString deserializeString(const char* valueString);
  VRIntArray deserializeIntArray(const char* valueString, const char separator);
  VRFloatArray deserializeFloatArray(const char* valueString,
                                       const char separator);
  VRStringArray deserializeStringArray(const char* valueString,
                                       const char separator);

  // This method creates a link from one node to another.  It has a
  // depth limit to guard against recursive definitions.
  bool linkNode(const std::string fullSourceName,
                const std::string fullTargetName, int depthLimit);

  // These are specialized set methods.  They seem a little unhip, but
  // it's because I find this easier than remembering how to spell the
  // pointers and casts.
  void setValueSpecialized(VRDatumPtr p, VRInt value) {
    p.intVal()->setValue(value);
  }
  
  void setValueSpecialized(VRDatumPtr p, VRFloat value) {
    p.floatVal()->setValue(value);
  }
  
  void setValueSpecialized(VRDatumPtr p, VRString value) {
    p.stringVal()->setValue(value);
  }
  
  void setValueSpecialized(VRDatumPtr p, VRIntArray value) {
    p.intArrayVal()->setValue(value);
  }
  
  void setValueSpecialized(VRDatumPtr p, VRFloatArray value) {
    p.floatArrayVal()->setValue(value);
  }
  
  void setValueSpecialized(VRDatumPtr p, VRStringArray value) {
    p.stringArrayVal()->setValue(value);
  }
  
  template <typename T, const VRCORETYPE_ID TID>
  std::string addDataSpecialized(const std::string valName, T value) {

    // All names must be in some namespace. If there is no namespace,
    // put this into the root namespace.
    std::string fixedValName = valName;
    if (valName[0] != '/') fixedValName = std::string("/") + valName;
      
    std::pair<VRDataMap::iterator, bool>res =
      mindex.insert(VRDataMap::value_type(fixedValName, NULL));

    // Was it already used?
    if (res.second) {

      VRDatumPtr obj = factory.CreateVRDatum(TID, &value);
      res.first->second = obj;

      // Add this value to the parent container, if any.
      VRContainer cValue;
      cValue.push_back(explodeName(fixedValName).back());
      std::string ns = getNameSpace(fixedValName);
      // The parent container is the namespace minus the trailing /.
      if (ns.compare("/") != 0) addData(ns.substr(0, ns.size() - 1), cValue);

    } else {

      // Entry already exists. Decide whether to modify or throw an exception.
      if (overwrite > 0) {

        setValueSpecialized(res.first->second, (T)value);

      } else if (overwrite == 0) {

        throw std::runtime_error(std::string("overwriting values not allowed"));
      }
    }
        
    return fixedValName;
  }


public:
  VRDataIndex();

  // Some constants that may be useful to users of this API.
  static std::string rootNameSpace;
  
  void setOverwrite(const int inVal) { overwrite = inVal; }

  // Returns the fully qualified name of the value.
  std::string getName(const std::string valName,
                      const std::string nameSpace);

  // Returns a pointer to the value with a given name (and namespace)
  VRDatumPtr getDatum(const std::string valName);
  VRDatumPtr getDatum(const std::string valName,
                      const std::string nameSpace);

  // The return type VRAnyCoreType is a wrapper that can hold any of the
  // VRCoreTypes.  VRAnyCoreType can be cast to any of the VRCoreTypes. So,
  // common usage examples for this function are:
  //
  //   VRInt i = dataIndex->getValue("MyInteger");
  //   VRFloat d = dataIndex->getValue("MyFloat");
  //
  // The same syntax can be used with custom classes that are not VRCoreTypes if
  // they implement a constructor that takes a VRAnyCoreType as an argument.
  // All of the classes in VRMath do this.  So, we can write:
  //
  //   VRVector3 v = dataIndex->getValue("MyHeading");
  //
  // You can also do something like this to return the specialized
  // VRDatum object which you might want for some other nefarious
  // reason.  I can't think of a use case for this, but you might.
  //
  //   VRDatumInt pobj = index->getValue(name, nameSpace).intVal()
  //   int p = index->getValue(name, nameSpace).intVal()->getValueInt()
  //
  VRAnyCoreType getValue(const std::string valName) {
    return getDatum(valName)->getValue();
  }
  VRAnyCoreType getValue(const std::string valName,
                         const std::string nameSpace) {
    return getDatum(valName, nameSpace)->getValue();
  }

  bool exists(const std::string valName, const std::string nameSpace) {
	  return getEntry(valName, nameSpace) != mindex.end();
  }

  bool exists(const std::string valName) {
	  return getEntry(valName) != mindex.end();
  }

  VRCORETYPE_ID getType(const std::string valName) {
    return getDatum(valName)->getType();
  }
  VRCORETYPE_ID getType(const std::string valName,
                        const std::string nameSpace) {
    return getDatum(valName, nameSpace)->getType();
  }

  std::string getTypeString(const std::string valName) {
    return getDatum(valName)->getDescription();
  }
  std::string getTypeString(const std::string valName,
                            const std::string nameSpace) {
    return getDatum(valName, nameSpace)->getDescription();
  }

  // This is the name, type, value, expressed as an XML fragment.
  std::string serialize(const std::string valName);
  std::string serialize(const std::string valName,
                        const std::string nameSpace);
  std::string serialize(const std::string name, VRDatumPtr pdata);

  // Takes a serialized bit of data and incorporates it into the data
  // index.
  std::string addSerializedValue(const std::string serializedData);
  std::string addSerializedValue(const std::string serializedData,
                                 const std::string nameSpace);
  // This one is just for testing and should usually be ignored.  The
  // boolean variable controls whether linkNode attributes are
  // expanded by invoking linkNodes().
  std::string addSerializedValue(const std::string serializedData,
                                 const std::string nameSpace,
                                 const bool expand);

  // Process the contents of a given XML file into the index.
  bool processXMLFile(const std::string fileName, const std::string nameSpace);
  bool processXMLFile(const std::string fileName);

  // Use this one at the start of a program.  It reads a file if
  // there's a file, and reads from a pipe if there's a pipe.
  bool processXML(const std::string arg);
  
  // Returns a list of all the names in the map.  Note this really is
  // a list of strings, not a VRContainer.  (No difference, really,
  // but we want to keep them semantically separate.)
  std::list<std::string> getNames();

  // Returns a list of names of objects with the given attribute.
  VRContainer selectByAttribute(const std::string attrName,
                                const std::string attrVal);
  VRContainer selectByAttribute(const std::string attrName,
								const std::string attrVal,
								const std::string nameSpace);
  VRContainer selectByType(const VRCORETYPE_ID typeId);
  VRContainer selectByName(const std::string inName);

  // Implements a 'linkNode' element in the config file, that copies a node
  // and all its contents.  Use it like this:
  //    <targetname linkNode="sourcename"/>
  // This will create an entry in the resulting data index with the
  // targetname linked to the *same* VRDatumPtr object as sourcename.
  bool linkNode(const std::string fullSourceName,
                const std::string fullTargetName) {
    return linkNode(fullSourceName, fullTargetName, 0);
  }

  // This does a global resolution of all linkNodes definitions.  It
  // is typically done as part of the process of incorporating some
  // XML, left public here for experimentation.
  bool linkNodes();

  // This is a roughly similar concept except that it copies the
  // contents of one namespace into another.  Looks for container
  // nodes with a 'linkContent' attribute and inserts into it links
  // from the specified container.
  bool linkContent();
  
  // The data index has a state that can be pushed and popped.  All
  // the changes to the index made after a pushState() can be rolled
  // back by calling popState().  This works by pushing and popping
  // values for each of the VRDatum objects represented in the index.
  void pushState();
  void popState();

  /// Step 6 of the data type addition instructions in VRDatum.h is to
  /// add a specialized method here, and to the setValue methods in
  /// the private section.
  std::string addData(const std::string valName, VRInt value);
  std::string addData(const std::string valName, VRFloat value);
  std::string addData(const std::string valName, VRString value);
  std::string addData(const std::string valName, VRIntArray value);
  std::string addData(const std::string valName, VRFloatArray value);
  std::string addData(const std::string valName, VRStringArray value);

  // There is a semantic difference between addData() for a primitive
  // value and addData() for a container.  One creates an object of
  // the name and value given.  The second (for containers) adds the
  // contents of the given container value to the container of the
  // given name.
  std::string addData(const std::string valName, VRContainer value);

  // A template for adding data.
  template<typename T>
  std::string addData(const std::string valName,
                      const std::string nameSpace, T value) {

    // If the name provided is an absolute name, ignore the nameSpace.
    if (valName[0] == '/') {
      
      return addData(valName, value);
    } else {
      
      return addData(validateNameSpace(nameSpace) + valName, value);
    }
  }
  
  // These versions of addData can be used by subclasses of the basic
  // VRDatum types.  If those types include a constructor from the basic
  // type, then this is all they need to be stored into an index and
  // retrieved.
  std::string addData(const std::string &name, VRIntConvertible &object) {
    return addData(name, object.toVRInt());
  }

  std::string addData(const std::string &name, VRFloatConvertible &object) {
    return addData(name, object.toVRFloat());
  }

  std::string addData(const std::string &name, VRStringConvertible &object) {
    return addData(name, object.toVRString());
  }

  std::string addData(const std::string &name, VRIntArrayConvertible &object) {
    return addData(name, object.toVRIntArray());
  }
  
  std::string addData(const std::string &name, VRFloatArrayConvertible &object) {
    return addData(name, object.toVRFloatArray());
  }
  
  std::string addData(const std::string &name, VRStringArrayConvertible &object) {
    return addData(name, object.toVRStringArray());
  }
  
  // A utility to make sure a namespace is spelled right, potentially
  // useful to users, so made public.
  std::string validateNameSpace(const std::string nameSpace);

  // Still another utility, to accommodate the use of environment
  // variables in the file names.  Also potentially useful, so public.
  static std::string dereferenceEnvVars(const std::string fileName);
  
  // Mostly just for debug purposes.
  std::string printStructure();
  std::string printStructure(const std::string itemName);
  std::string printStructure(const std::string itemName, const int lim);
  // Prints a vaguely tree-ish representation of an XML parse.  Just
  // an aid to debugging, really.
  bool printXML(element* node, std::string prefix);

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
//        vector<float> [DONE]
//        matrix<float> [TBD]
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
//   - Improve (and test) adding values to containers. [DONE]
//
//   Need to simplify the process of adding data types.
//
//   - Make CreateVRDatum functions into templates?
//
//   - Make VRDatumPtr::intVal(), etc, into templates.
//
//   - After those are done, the VRDataIndex::addData() methods can
//     probably be template-ized, too.
//
//   - An audit of all the calls might be good, to look into
//     reference-izing anything that is actually called by value and
//     shouldn't be.
//
//   - We are adding a "list" of index objects.  These are an ordered
//     collection of VRDataIndex objects, all with more or less the
//     same name, but differentiated by index.  They can be as motley
//     a collection of objects as you care to try to manage, but
//     managing them is your problem.  We will refer to this as an
//     "imaginary" type called VRList.
//
//     This is implemented by redefining the data index typemap to
//     point to a std::vector of VRDatumPtr objects.  It will require
//     some extra parsing of an index name, but that's all.  Hardest
//     part is the semantics of reading stuff in -- overwrite or just
//     add to list?
//
//     Will need:
//
//       - Add std::vector<VRDatumPtr> to the typemap instead of
//         VRDatumPtr.
//
//       - Make work for single element sets.
//
//       - Provide some parsing aids to look for subscripts on the
//         ends of names.  Perhaps names are no longer a string, but a
//         string and a subscript value struct?
//
//       - Probably need a couple of management aids, like a size()
//         method and something like an iterator.
//
}  // end namespace MinVR
#endif
