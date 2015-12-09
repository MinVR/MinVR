#ifndef VRDATAINDEX_H
#define VRDATAINDEX_H

#include <map>
#include <list>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <sstream>

#include "VRDatum.h"
#include "VRDatumFactory.h"

// forward declaration for Cxml
class element;


/**
  Design Notes (Dan 11/5/15):

  Let's clarify and be as consistent as possible with our terminology:

  VRCoreTypes:  VRInt, VRDouble, VRString, VRIntArray, VRDoubleArray, VRStringArrary, VRContainer?
    - These are just defined by typedefs, we don't want extra overhead here.
    - Might want to add VRBytes in the future
    - Other than that, we think this is a pretty complete list of basic types that could be combined
      together to represent almost any kind of data.
    - Design question:  Does VRContainer really belong in this list, or is it just an artifact of 
      the way that VRDataIndex is implemented?

  VRCORETYPE_ID: an enum with an unique ID for each of the VRCoreTypes

  VRDatum:  An object that stores a (VRCORETYPE_ID, VRCoreType) pair.
    - VRDatum is an abstract class and there is a concrete implementation for each of the VRCoreTypes.
    - (I think it's the case that) 99% of the time, programmers using the MinVR API don't need to know 
      that VRDatums are used under the hood because they will always access data through a VRDataIndex.

  VRDataIndex:  This is the flexible and extensible "data struct" that we have longed for for many years :)
    - Internally, data are stored in (name, VRDatumPtr) pairs.
    - Externally, programmers will think of this more as (name, value), where value can be stored in any of
      the VRCoreTypes, and the type is usually inferred automatically by the context.  Often, programmers 
      will know what VRCoreType to expect based upon the name.  For example the value stored under the name
      "NumWindows" should be a VRInt.  But, it is also possible to query the type for a specific name.
    - Likewise, programmers will often know what names to expect in a VRDataIndex.  For example, we may know
      that the DataIndex for a Mouse_Move will always include an entry named "Position".  But, it is also
      possible to query the names of all of the entries in the index.
    - The entire DataIndex can be serialized, or just specific entries can be serialized.  The current
      method of serialization is via a std::string in XML format.  We might also want to support serializing
      to a binary format in the future.  The current serialization methods are intentially named toXML()
      and fromXML() rather than serialize(), deserialize() to plan for this flexibility.


This class is meant to hold a data object of some arbitrary type (ok, arbitrary
within a small range of types), and to serialize and deserialize it into a
query-able form suitable for transmission. It holds a type and a value.  The
idea is that all VR data types can be addressed as a VRDatum object (well,
technically as the reference for a VRDatumPtr pointer object), for convenient
loading of heterogeneous types in a homogeneous way, such as in a map or list.
This class only holds a value; the names must be recorded elsewhere.

The data for each object is stored both in its native form, and as a serialized
data string.  It can be constructed from either form, so the serialized string
can be sent across the network and reconstructed to another datum object on the
other side.

To extend the collection of data types that can be modeled as a VRDatum, follow
these steps:

1. Add an entry in the MVRTYPE_ID enum above, if necessary, and
   the initialization of the MVRTypeMap in VRDatum.cpp.

2.  Add a conversion to the VRDatumHelper class.  You'll
    probably want to make your type a typedef if you haven't
    already.  Put it up there with MVRContainer.

3. Create a specialization of the VRDatum class, and call it
   something like VRDatumInt or VRDatumDouble.  You will
   have to provide it with a private value member and public
   getValue*() and getValue() method members.  (The latter are
   all the same, so that's no big deal.)  Add an entry to the list of
   virtual members for these functions, too.

4.  You will also need a setValue() function.  These objects are
    meant to be immutable, but when a new name-value pair has the
    same name as another pair it seems a shame to create an
    entirely new object to hold the new value when an
    already-existing one is right there.  So when it is
    appropriate for a new name to supplant the old, we can
    provide some small degree of optimization by just changing
    the value of the object.  So these objects are only sort of
    immutable in the sense that we're supposed to pretend they
    are, and so long as no one lets on, the secret will be safe.
    Ok?  The syntax for using setValue() is annoying, so users
    shouldn't really be using it anyway.

5. Add a method to the VRDatumPtr that will return the new
   data type.  See intVal() and doubleVal() for models.

6. Add a "create" function for the factory to use.  Something
   like these:

     VRDatumPtr CreateVRDatumInt(void *pData);
     VRDatumPtr CreateVRDatumDouble(void *pData);

7. Then add this number and the create function to the list of
   data types registered in the constructor for the
   VRDataIndex class.

8. Add an addValue() method to the VRDataIndex class that
   accepts one of the new data types, creates a VRDatum object
   for it, and adds a pointer to it in the data index.

9. Write a deserialize*() method for VRDataCollection and find
   a place for it in the processValue() method of that object.

10. You should also consider adding something to
   VRDataIndex::inferType to identify the new data type.  This
   is pretty free-form, but follow the models in there.  This is
   also not necessary, but if you don't do it, your data types
   will always require a 'type="XX"' attribute when specified,
   and may not give obvious errors if you don't do that.
*/
class VRDataIndex {
public:

  VRDataIndex();

  static VRDataIndex fromXML(const std::string &xmlString);

  
  virtual ~VRDataIndex() {}

  
  /// ***** ADDING DATA TO THE INDEX *****

  /// Use the following to add data into the DataIndex.  This will create
  /// a new (name, (type, value)) entry in the index.
  std::string addData(const std::string &name, VRInt value);
  std::string addData(const std::string &name, VRDouble value);
  std::string addData(const std::string &name, VRString value);
  std::string addData(const std::string &name, VRIntArray value);
  std::string addData(const std::string &name, VRDoubleArray value);
  std::string addData(const std::string &name, VRStringArray value);
  std::string addData(const std::string &name, VRContainer value);

  // Tom: Should this be called something like addNamespace?
  std::string addData(const std::string &name);


  void addData(const std::string &name, VRIntConvertable &object) {
    addData(name, object.toVRInt());
  }

  void addData(const std::string &name, VRDoubleConvertable &object) {
    addData(name, object.toVRDouble());
  }

  void addData(const std::string &name, VRStringConvertable &object) {
    addData(name, object.toVRString());
  }

  void addData(const std::string &name, VRIntArrayConvertable &object) {
    addData(name, object.toVRIntArray());
  }
  
  void addData(const std::string &name, VRDoubleArrayConvertable &object) {
    addData(name, object.toVRDoubleArray());
  }
  
  void addData(const std::string &name, VRStringArrayConvertable &object) {
    addData(name, object.toVRStringArray());
  }

  
  
  // ***** ACCESSING DATA STORED IN THE INDEX *****
  
  
  /// **** Access to the names

  /// Returns a list of all the names in the map.
  std::list<std::string> getNames();
  /// Returns a list of all the names in the map within the specified namespace.
  std::list<std::string> getNames(const std::string &nameSpace);
  std::list<std::string> getNames(const std::string &nameSpace, bool includeChildren, bool fullPath);


  /// **** Access to the value stored under the specified name

  /// The return type VRAnyCoreType is a wrapper that can hold any of the
  /// VRCoreTypes.  VRAnyCoreType can be cast to any of the VRCoreTypes. So,
  /// common usage examples for this function are:
  ///   VRInt i = dataIndex->getValue("MyInteger");
  ///   VRDouble d = dataIndex->getValue("MyDouble");
  /// The same syntax can be used with custom classes that are not VRCoreTypes if
  /// they implement a constructor that takes a VRAnyCoreType as an argument.
  /// All of the classes in VRMath do this.  So, we can write:
  ///   VRVector3 v = dataIndex->getValue("MyHeading");
  VRAnyCoreType getValue(const std::string fullName) {
    return getDatumPtr(getTrimName(fullName), getNameSpace(fullName))->getValue();
  }
  VRAnyCoreType getValue(const std::string name, const std::string nameSpace) {
    return getDatumPtr(name, nameSpace)->getValue();
  }

  /// Returns a pointer to the value with a given name (and namespace).  This
  /// is the method to use if you need to modify a value stored in the index.
  VRDatumPtr getDatumPtr(const std::string name);
  VRDatumPtr getDatumPtr(const std::string name, const std::string nameSpace);
  

  /// **** Access to the type stored under the specified name

  /// Returns the type associated with the name
  VRCORETYPE_ID getType(const std::string &fullName);
  std::string   getTypeAsString(const std::string &fullName);
  std::string   getTypeAsString(const std::string &name, const std::string &nameSpace);


  
  /// ***** TO/FROM XML *****
  
  // Takes a serialized bit of data and incorporates it into the data index.
  bool addDataFromXML(const std::string serializedData);
  bool addDataFromXML(const std::string serializedData, const std::string nameSpace);
  /// Process the contents of a given XML file into the index.
  bool addDataFromXMLFile(std::string fileName, std::string nameSpace);

  /// This serializes the entire DataIndex using XML.  The string will begin with
  /// <VRDataIndex> and end with </VRDataIndex>.
  std::string toXML();
  
  /// This is the name, type, value, expressed as an XML fragment for just the specified name
  std::string dataToXML(const std::string fullName);
  std::string dataToXML(const std::string name, const std::string nameSpace);
  std::string dataToXML(const std::string name, VRDatumPtr pdata);
  
  
  /// ***** UTILITY FUNCTIONS *****
  
  /// A utility to make sure a namespace is spelled right and fix it if it is
  /// missing a leading /, potentially useful to users, so made public.
  std::string validateNameSpace(const std::string nameSpace);

  /// Mostly just for debug purposes.
  void printStructure();

  void setOverwrite(const int inVal) { overwrite = inVal; }
    
    void setDefaultNamespace(const std::string& val)
    {
        defaultNamespace = val;
    }
    
    void resetDefaultNamespace()
    {
        defaultNamespace = "/";
    }


private: // ****** IMPLEMENTATION DETAILS BELOW THIS POINT ******
    
    // Just a utility to return the tail end of the fully qualified name.
    // i.e. trimName("cora/flora", "/bob/nora") is "flora".  This does not
    // look in the index at all, just manipulates strings.
    std::string getTrimName(const std::string valName, const std::string nameSpace);
    std::string getTrimName(const std::string valName);

  // Another utility, meant to pull a name apart on the slashes.
  std::vector<std::string> explodeName(const std::string fullName);

  // Returns the namespace, derived from a long, fully-qualified, name.
  std::string getNameSpace(const std::string fullName);

  
  typedef std::map<std::string, VRDatumPtr> VRDataMap;
  VRDataMap mindex;
  
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
  
  VRInt         deserializeInt(const char* valueString);
  VRDouble      deserializeDouble(const char* valueString);
  VRString      deserializeString(const char* valueString);
  VRIntArray    deserializeIntArray(const char* valueString);
  VRDoubleArray deserializeDoubleArray(const char* valueString);
  VRStringArray deserializeStringArray(const char* valueString);
  
  
  bool processValue(const char* name, VRCORETYPE_ID type, const char* valueString);
  VRCORETYPE_ID inferType(const std::string valueString);
  bool walkXML(element* node, std::string nameSpace);
  

  // If this is 1, new values will overwrite old ones.  For -1, new
  // values will just bounce off.  And zero will cause an exception if
  // an overwrite is attempted.  Except containers, who are always
  // happy to receive new values and add to their existing list.  The
  // default class constructor allows overwrites.
  int overwrite;
  
  VRDatumFactory factory;

  // This is just a convenience to map strings to object type numbers.
  std::map<std::string, VRCORETYPE_ID> vrTypeMap;
    
  std::string defaultNamespace;
  
};




#endif
 
