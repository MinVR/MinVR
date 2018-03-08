// -*-c++-*-
///
/// \file
/// This file is part of the MinVR Open Source Project, which is
/// developed and maintained collaboratively by the University of
/// Minnesota and Brown University.
///
/// Copyright (c) 2016 Regents of the University of Minnesota and
/// Brown University.
///
/// This software is distributed under the BSD-3 Clause license, which
/// can be found at: MinVR/LICENSE.txt.
///
///
/// Original Author(s) of this File:
///  	Tom Sgouros 8/29/2015, Brown University
///
/// Author(s) of Significant Updates/Modifications to the File:
///	...
///

#ifndef MINVR_DATAINDEX_H
#define MINVR_DATAINDEX_H

#include "VRDatumFactory.h"
#include "Cxml/Cxml.h"
namespace MinVR {

/// \brief A dynamically-typed data store.
///
///  This class implements a VRDataIndex, a flexible structure that stores
///  multiple data fields as key=value pairs, like an associative array, but
///  with several significant enhancements.
///
///    - Multiple types -- in C++ it is a challenge to create an associative
///      array where the data stored in the array can be any type.  VRDataIndex
///      addresses this by supporting an extensible set of core types defined
///      in VRCoreTypes.h.  Unlike in C++ *values* have type, but *names* do
///      not.
///
///    - Hierarchical scoping -- One of the data types in the VRDataIndex is a
///      "container", comparable to a struct, that contains other data values,
///      potentially including other containers.  The containers define a
///      "namespace" or a "scope" and the same name may appear within multiple
///      containers.  Inner namespaces inherit all of the fields defined in
///      outer namespaces.  When data are accessed from an inner namespace,
///      locally defined data fields override outer fields with the same name.
///
///    - Serialization -- The entire data index, or some piece of it, can be
///      read from and written to an XML file format, for ease of passing over
///      a network.
///
///    - State -- The entire data index state can be preserved by pushing it
///      onto a stack, and later restored by popping that stack.
///
///  MinVR reports event and render state information to API users via
///  VRDataIndex objects. VRDataIndex is also used extensively inside MinVR,
///  including to store data read in from config files.  Users can create their
///  own VRDataIndex objects if they wish.
///
///  VRDataIndex objects store a collection of data fields as key=value pairs
///  where the keys are std::strings and the values can be any of the MinVR
///  core types defined in VRCoreTypes.h.  An important data type is the
///  "container", a struct, more or less, that defines a "namespace" or
///  "scope" for a data field name.
///
///
///  ## Hierarchical Scoping
///
///  Unlike standard associative arrays, the key string can include the name
///  of a container holding some data.  Think of the data values as existing
///  inside one of a collection of nested containers.  You name the container,
///  and then the value inside it, and you can specify the whole thing by
///  specifying the container name, appended with a slash (/) and the data
///  field name.  Containers and the namespaces they define constitute an
///  inheritance hierarchy used for data lookups.  You'll recognize the
///  namespace concept from C++, and the way the inheritance works is similar
///  to how global and local variables work in C++, too:
///
///    ~~~
///    // Global scope
///    int width = 10;
///    int height = 10;
///
///    void extraWideWindow() {
///      // Defining width in this local function overrides access to global width
///      int width = 20;
///      // Accessing width and height from within the local function picks up the
///      // global version of height and the local version of width
///      cout << width << ", " << height << endl;  // prints 20, 10
///    }
///    ~~~
///
///  An analogous example using a VRDataIndex looks like this:
///    ~~~
///    VRDataIndex index();
///
///    // Default case: windows are a 10 x 10 square
///    index.addData("Width", 10);
///    index.addData("Height", 10);
///
///    // There is also a special case of an extra wide window.  The width
///    // changes but all other window settings are the same.
///    index.addData("ExtraWideWindow/Width", 20);
///
///    // Here's how to retrieve data for the default window case
///    int wdefault = index.getValue("Width");
///    int hdefault = index.getValue("Height");
///
///    // Here's how to retrieve data for the extrawide window case
///    int wwide = index.getValue("ExtraWideWindow/Width");
///    int hwide = index.getValue("ExtraWideWindow/Height");
///    ~~~
///
///  There are two important guidelines to note in this example:
///
///    - First, to add or get data from within a container, simply include the
///      container name (the namespace) within the key passed to the addData() and
///      getValue() functions.  Separate the container and the data field name
///      with /'s.  There is no limit to the depth of nesting.
///
///    - Second, notice in the last two lines of the example that *both* lines
///      request data from the ExtraWideWindow namespace even though Height was
///      never defined there.  This is because Height (and any other common
///      window settings) will be automatically inherited.  This is a common
///      usage pattern since VRDataIndex objects are often used for configuring
///      displays that have many common settings but some variation (e.g.,
///      multiple projectors that are part of a large display wall).
///
///  When working with a data index that might hold many default values and a
///  few special cases, remember to:
///
///   1. To avoid duplicating data, put as much common data as you can within
///      the root level -- the global namespace,
///
///   2. Override values in local containers as needed, and
///
///   3. When retrieving the data, use the namespace defined by the container
///      to access your data.  This will allow you to take advantage of the
///      inheritance feature.
///
///  If you do this, then your code should only need to know which namespace to
///  use, not which specific data fields change from one special case to the
///  next.
///
///
///  ## Storing your own classes in VRDataIndex
///
///  The MinVR core types were selected with the idea that most data structures
///  commonly used in VR apps can be stored in one or a combination of core
///  types.  For example, a 3D point, vector, quaternion, or 4x4 transformation
///  matrix can all be stored in a std::vector<float>, and the data index
///  supports a VRFloatArray type to encapsulate that.
///
///  If you use your own C++ classes for points, vectors, matrices, or other
///  objects you would like to store in a VRDataIndex, you can extend your
///  classes to make them nicely compatible with VRDataIndex by following these
///  two steps: (1) Make your class implement one of the VR*Convertible
///  interfaces defined in VRCoreTypes.h.  This will allow the data index to
///  convert your class's data into a core type can can be stored in a the
///  index.  (2) Implement a constructor that takes the corresponding core type
///  as an argument. If you do these two things, then you can use the data
///  index with your class as follows:
///    ~~~
///    MyClass m1;
///    dataIndex->addData("MySpecialKey", m1);
///    //...
///    MyClass m2 = dataIndex->getData("MySpecialKey");
///    ~~~
///
///  ## XML Representation
///
///  A VRDataIndex can read/write itself or a subset of itself to an XML
///  format.  Each named data field is represented as an XML element where the
///  XML start tag and end tag are the data's name and the content of the
///  element describes the data.  Here is an example that uses a variety of
///  core types to store data fields associated with a touch input event:
///    ~~~
///    <SensorName type="string">MultiTouchTablet</SensorName>
///    <TouchID type="int">1</TouchID>
///    <Speed type="float">1.025</Speed>
///    <PixelPosition type="intarray">100, 125</PixelPosition>
///    <NormalizedPosition type="floatarray">0.5, 0.65</NormalizedPosition>
///    <Modifiers type="stringarray">Shift,Ctrl,Alt</Modifiers>
///    ~~~
///  The `type=` attribute is an optional directive for the parser.  It would
///  have been fine to leave it out of the example above because the type can
///  be inferred from the content in all but extreme cases (e.g., a string that
///  contains only numbers).
///
///  Containers are also represented as XML elements.  The start tag and end
///  tag use the name of the container (and thus the namespace of the contained
///  objects).  Container elements contain child elements, which can be either
///  other container elements or data elements.
///
///  The XML parser used here is based on the "Simple C++ XML Parser" from the
///  CodeProject article by "BratilaRazvan", 2010.
///  see: http:///www.codeproject.com/Articles/111243/Simple-Cplusplus-XML-Parser
///  It does not currently support entities, or CDATA declarations.
///
///
///  ### Serializing an Entire Index
///
///  When an entire VRDataIndex is serialized, the index's name is used for the
///  start and end tags, and all the root-level containers and data appear as
///  children of that top-level. The extrawide window example from above would
///  be represented as:
///    ~~~
///    <MyWindowSettings type="container">
///        <Width type="int">10</Width>
///        <Height type="int">10</Height>
///        <ExtraWideWindow>
///            <Width type="int">20</Width>
///        </ExtraWideWindow>
///    </MyWindowSettings>
///    ~~~
///
///  ### Using XML Attributes
///
///  In addition to the `type=` attribute, which is reserved for internal use,
///  user-defined attributes can also be set and retrieved.  This feature can
///  be used to tag data values that you wish to easily retrieve later.  For
///  example, if you set an attribute of `nodeType="WindowNode"` for all of the
///  WindowNodes stored in your index, then you can easily retrieve a list of
///  all entries that represent WindowNodes by calling
///    ~~~
///    index->selectByAttribute("nodeType", "WindowNode");
///    ~~~
///  Aside from this "tagging for ease of later retrieval" feature, which is
///  most often used as a way to write cleaner XML config files, attributes are
///  not intended to be used to store significant data values, since this is
///  the job of the rest of the data index.  Only std::string values are
///  supported for attribute values.
///
///  For the most part, the data index is agnostic about attributes.  You are
///  in charge of naming them and retrieving them.  However, there is one other
///  special attribute, a "separator" attribute used to define the serialized
///  representation of array data.  It works like this:
///    ~~~
///    <cora type="intarray">3,4,5,6</cora>
///    <nora type="stringarray" separator="@">A string@B sting@C sing@D sin@E in@F i</nora>
///    ~~~
///  If no separator is specified, the default is a comma ','.  You can use
///  backslash (\) to include a separator character in one of the strings.  So:
///    ~~~
///    <cora>Alpha,Beta,Gamma\,Delta,Epsilon</cora>
///    ~~~
///  Comes out with four entries in the array, "Alpha", "Beta", "Gamma,Delta",
///  and "Epsilon".  See indextest.cpp for an example.
///
///
///
///  ## More about Containers and Namespaces
///
///  To use the data index effectively, it is useful to understand the concept
///  of a namespace, or scope.  If you don't want to bother, just remember
///  this: a namespace is just a container's name, with a '/' after it.  The
///  rest is just detail.
///
///  Containers define a namespace as well as hold a bunch of objects inside
///  them.  The two are very closely related, but are conceptually distinct.
///  From a user's point of view, a namespace is a string that starts and ends
///  with a '/' and may or may not have some text in the middle.  Here are some
///  possible namespaces:
///     ~~~
///     /perry/
///     /freida/mary/
///     /     <- the root namespace to which all VRDataIndex objects belong.
///     /homer/henry/martha/
///     ~~~
///  As with a Linux directory, a data value might be specified with a complete
///  name, such as /homer/henry/martha, or as a relative name, 'martha' defined
///  relative to the /homer/henry namespace.
///
///  A container object is simply an object that "contains" other values.
///  Those objects can be core data types, or they can also be other
///  containers.  The container object contains a list of the (relative) names
///  of the objects it contains.  Here is a possible container:
///     ~~~
///     | peter
///     | | harold
///     | | norma
///     | | flora
///     ~~~
///  This corresponds to an index that contains these names:
///
///     ~~~
///     /peter
///     /peter/harold
///     /peter/norma
///     /peter/flora
///     ~~~
///
///  Note that the container lists the relative name.  When the container is
///  serialized, the serialized version contains only the relative names.
///
///  ## Usage
///
///  To use a VRDataIndex, create an object of that class.  You can use the
///  default constructor, which will give you an empty index with the default
///  name of "MVR", ready for action.  You can also use a constructor that
///  accepts a serialized data index as an XML-encoded string, which loads the
///  data into the index, ready to go.  The root name of the serialized string
///  will become the name of the data index object, and the children of that
///  root name will be at the root level of the data index.
///    ~~~
///    VRDataIndex *m = new VRDataIndex();  // Gets you an empty index.
///
///    // The following will create an index named 'a' containing
///    // data values 'b' and 'c', both integers
///    VRDataIndex *m = new VRDataIndex("<a><b>3</b><c>5</c></a>")
///    ~~~
///  Some other examples of serialized XML data, ready to become VRDataIndex
///  values:
///    ~~~
///    <flora type="int">42</flora>
///
///    <bob type="container">
///       <flora type="int">42</flora>
///       <pi type="float">3.1415926</pi>
///    </bob>
///    ~~~
///  You can add values to the index in one of three ways.
///
///   1. With the addData() functions.  These take a name and a value and park
///      them in the index.  The addData() with a single name argument adds a
///      container of that name and you can subsequently add items to it.
///
///   2. Feed some serialized data to the addSerializedValue() functions.  This
///      is just XML-encoded string data, such as is output by serialize().
///
///   3. Feed a file containing XML into processXMLFile().  There is no
///      validation of the XML done, and there are some ways in which this is
///      not an arbitrary XML parser.  For example, the parser assumes
///      uniqueness of names within an element.  Two elements of the same name
///      within some parent element will overwrite one another.  Validation of
///      the XML is left to some other process, hopefully with a schema that
///      enforces this limitation.
///
///  Once an index has entries, they can be retrieved at your pleasure with
///  getValue() and serialize().  The getValue() method returns an object that
///  can be used directly in your program.  The casts are built into the
///  function via a helper class, so long as the compiler can figure out the
///  appropriate type to convert to.  So (int)p->getValue() gets you an integer
///  and (float)p->getValue() gets you a float. (So long as the relevant
///  objects actually contain an integer and float, otherwise you get an
///  error.)
///
///  To use, do this:
///    ~~~
///    VRDataIndex *index = new VRDataIndex;
///    int a = 4;
///    index->addData("/george", a);
///    ~~~
///  This incorporates an integer value of 4 into the index with the name
///  "george" that appears at the root (global) level.
///
///  When you want to refer to the value you put in, do this:
///    ~~~
///    int p = index->getValue("george");
///    ~~~
///  or
///    ~~~
///    int p = index->getValue("george", "/");
///    ~~~
///  Where the second argument is the "namespace", which is arranged
///  like a directory structure, with slashes (/) separating the names.  You
///  can also do this:
///    ~~~
///    std::string s = index->serialize("/george")
///
///    VRDataIndex *newIndex = new VRDataIndex;
///    newIndex->addSerializedValue(s);
///    int b = newIndex->getValue("/george");
///    ~~~
///  The b variable now contains the same value as a, with the same name.
///
///
///  There is an exercz.cpp file that does an ok job of illustrating some of
///  the usage.  You'll find it filed under "interactive tests" as the
///  config-viewer.  You can run that program to learn about the namespace and
///  name scoping concepts.  It's sort of like an interpreter for some
///  primitive language environment whose only functions are assigning
///  variables and reading them.
///
///  The system encompasses the VRDataIndex class, whose job it is to track a
///  set of names and corresponding VRDatumPtr objects, smart pointers to
///  VRDatum objects, that hold a type and value.  There is also a
///  VRDatumFactory object, whose only use is creating the specializations of
///  the VRDatum object for each of the supported data types.  It's all meant
///  to be as portable as possible and not rely on any external libraries.  It
///  uses an XML reader, see those files for the original credit.
///
class VRDataIndex {

public:
  /// \brief Creates an empty index.
  ///
  /// The created index has the default name "MVR", which can be changed with
  /// setName().  The index name is used when the index is serialized.
  VRDataIndex()  : _indexName("MVR"), _overwrite(1), _linkNeeded(false) {
    _lastDatum = _theIndex.end();
  }

  /// \brief Creates an index containing the given data.
  ///
  /// And another that fills an index with some serialized data.  The
  /// root name in the serialized data is adopted as the '_indexName' field
  /// of the index object.
  ///
  /// If the input data is just a simple string with no XML features
  /// (i.e. it does not begin with a '<') then just create an empty
  /// index with the given name.
  VRDataIndex(const std::string serializedData);

  /// \brief Makes a deep copy.
  ///
  /// The copy constructor makes a deep copy (all the way down) of all the data
  /// in the data index.
  VRDataIndex(const VRDataIndex &orig);

  /// \brief Also makes a deep copy.
  ///
  /// Like the copy constructor, the assignment operator makes a deep copy,
  /// too.  Note that the argument is not a reference (i.e. it's a copy
  /// created by the copy constructor).
  VRDataIndex& operator=(const VRDataIndex rhs) {

    _theIndex = rhs._theIndex;
    _indexName = rhs._indexName;
    _overwrite = rhs._overwrite;
    _linkRegister = rhs._linkRegister;
    _linkNeeded = rhs._linkNeeded;

    return *this;
  };

  /// \brief Destructor.
  ///
  /// Because the index uses smart pointers to the VRDatum objects, the default
  /// destructor is all that is needed.
  ~VRDataIndex() {};

  /// \brief Returns the name of the data index.
  std::string getName() const { return _indexName; };

  /// \brief Changes the name of the data index.
  void setName(const std::string indexName) { _indexName = indexName; };

  ///@{
  /// \name Add data
  ///
  /// These methods add a key=value pair to the index, where the value is
  /// of one of the VRCoreTypes.
  ///
  /// Examples:
  ///   ~~~
  ///   index.addData("Width", 10);             // adds to global namespace
  ///   index.addData("ExtraWide/Width", 20);   // adds to ExtraWide/ namespace
  ///   index.addData("ExtraWide/2/Width", 20); // adds to ExtraWide/2/ namespace
  ///   ~~~
  ///
  /// The addData() methods include versions of addData to be used with
  /// subclasses of the basic VRDatum types.  If those types include a
  /// constructor from the basic type, then this is all they need to be stored
  /// into an index and retrieved.
  ///
  ///
  /// \param key The key to store this data value under.  This must include
  /// the name of the data field.  Optionally, one or more namespaces may be
  /// prepended to the name separated by /'s.
  /// \param value The VR core type data value to store under the given key.
  std::string addData(const std::string &key, VRInt value);

  /// \copydoc VRDataIndex::addData(const std::string &key, VRInt value);
  std::string addData(const std::string &key, VRFloat value);

  /// \copydoc VRDataIndex::addData(const std::string &key, VRInt value);
  std::string addData(const std::string &key, VRString value);

  /// \copydoc VRDataIndex::addData(const std::string &key, VRInt value);
  std::string addData(const std::string &key, VRIntArray value);

  /// \copydoc VRDataIndex::addData(const std::string &key, VRInt value);
  std::string addData(const std::string &key, VRFloatArray value);

  /// \copydoc VRDataIndex::addData(const std::string &key, VRInt value);
  std::string addData(const std::string &key, VRStringArray value);


  /// \copydoc VRDataIndex::addData(const std::string &key, VRInt value);
  ///
  /// There is a semantic difference between addData() for a primitive
  /// value and addData() for a container.  One creates an object of
  /// the name and value given.  The second (for containers) adds the
  /// contents of the given container value to the container of the
  /// given name.
  std::string addData(const std::string &key, VRContainer value);

  /// \brief Add a key=value pair to the index.
  ///
  /// Takes a string of the form 'key=value' and loads the value into the data
  /// index using 'key' as the name.  No namespace is consulted, what you
  /// specify is what you get.
  std::string addData(const std::string &keyAndValue);


  /// \brief A template for adding data.
  ///
  /// This exists for those who want to let the compiler guess the data type of
  /// the argument.  Works fairly well.  Most of the time.  You are warned.
  template<typename T>
  std::string addData(const std::string &key,
                      const std::string nameSpace, T value) {

    // If the name provided is an absolute name, ignore the nameSpace.
    if (key[0] == '/') {

      return addData(key, value);
    } else {

      return addData(validateNameSpace(nameSpace) + key, value);
    }
  }

  /* 1b. Methods for adding data from classes that implement a VR*Convertible
     interface. */

  /// For classes that implement the VRIntConvertible interface.
  std::string addData(const std::string &name, const VRIntConvertible &object) {
    return addData(name, object.toVRInt());
  }

  /// For classes that implement the VRFloatConvertible interface.
  std::string addData(const std::string &name, const VRFloatConvertible &object) {
    return addData(name, object.toVRFloat());
  }

  /// For classes that implement the VRStringConvertible interface.
  std::string addData(const std::string &name, const VRStringConvertible &object) {
    return addData(name, object.toVRString());
  }

  /// For classes that implement the VRIntArrayConvertible interface.
  std::string addData(const std::string &name, const VRIntArrayConvertible &object) {
    return addData(name, object.toVRIntArray());
  }

  /// For classes that implement the VRFloatArrayConvertible interface.
  std::string addData(const std::string &name, const VRFloatArrayConvertible &object) {
    return addData(name, object.toVRFloatArray());
  }

  /// For classes that implement the VRStringArrayConvertible interface.
  std::string addData(const std::string &name, const VRStringArrayConvertible &object) {
    return addData(name, object.toVRStringArray());
  }

  /// \brief Controls whether new values overwrite old.
  ///
  /// This controls behavior when addData() or addSerializedData() encounters
  /// a name that is already in the data index.  There are three
  /// possibilities: either quietly overwrite the existing value (1), don't
  /// overwrite the existing value (-1), or throw an exception (0).
  ///
  /// This setting is not relevant to container objects, who are always happy
  /// to receive new values and add to their existing list.  The class
  /// constructor default is to allow overwrites.
  ///
  /// \param overwrite If this is 1, new values will overwrite old ones.  For
  /// -1, new values will just bounce off, leaving the index unchanged.  Set
  /// this to zero to cause an exception if an overwrite is attempted.
  void setOverwrite(const int overwrite) { _overwrite = overwrite; }

  ///@}


  ///@{
  /// \name Accessing data values
  ///
  /// There are two groups of functions for accessing data values: the
  /// `get*()` methods and the `selectBy*` methods.  The `get*` methods take
  /// advantage of the inheritance feature of the data index and return a
  /// *single* name in response to a query.  If you call getValue() with a
  /// simple data name, you'll get the value associated with that name if that
  /// name is in the given namespace/container.  If the given name is not in
  /// the container, the data index looks in the container containing it, and
  /// then in the container containing that one, and so on up the chain of
  /// nested containers.  If you don't want to deal with the inheritance
  /// feature, use the inherit argument to turn it off.
  ///
  /// For more targeted selection of data values, or to return a collection of
  /// names, consider the `selectBy*` methods, which use other criteria to
  /// find values.
  ///

  /// \brief Returns the value associated with some name.
  ///
  /// Returns the value of the data field stored under key.  The return type
  /// VRAnyCoreType is a wrapper that can be cast to any core type.
  /// Do not use VRAnyCoreType directly; instead just cast it immediately to
  /// whatever core type you expect your call to getValue() to return.  C++
  /// will do the cast automatically when you use getValue() as follows:
  /// ~~~
  /// VRInt i = dataIndex->getValue("MyInteger");
  /// VRFloat d = dataIndex->getValue("MyFloat");
  /// ~~~
  /// Note, the same syntax can be used with custom classes that are not
  /// VRCoreTypes if they implement a constructor that takes a VRAnyCoreType
  /// as an argument.  This allows you to write:
  /// ~~~
  /// MyClass m = dataIndex->getValue("MyKey");
  /// ~~~
  /// All of the classes in VRMath do this, so you can look there for examples.
  ///
  /// A VRError is thrown if key does not exist in the index.
  ///
  /// \param key The name of the data field to retrieve.  The name may include
  /// namespaces, separated by slashes.  Valid names are strings like "cora",
  /// "nora/cora", and "/flora/nora/cora".  The last name is "fully qualified,
  /// which means it begins with a slash.  In this case, the function only
  /// returns a value if that exact name appears in the data index and the
  /// nameSpace argument is ignored, if it is provided.
  /// \param nameSpace The container in which to look for the key.
  /// \param inherit Set to false if you only want values from the specified
  /// container/namespace and not inherit from any namespace above.
  VRAnyCoreType getValue(const std::string &key,
                         const std::string nameSpace = "",
                         const bool inherit = true) const {
    VRDataMap::iterator p =
      const_cast<VRDataIndex*>(this)->_getEntry(key, nameSpace, inherit);

    if (p == _theIndex.end()) {
      if (nameSpace.empty()) {
        VRERRORNOADV("Never heard of " + key + ".");
      } else {
        VRERRORNOADV("Never heard of " + key + " in namespace " + nameSpace + ".");
      }
    } else {
      return p->second->getValue();
    }
  }

  /// \brief Returns the last value requested.
  ///
  /// Use this version to achieve a slight optimization.  If you do a
  /// getEntry(), the result will be stored in _lastDatum, so you can
  /// use this call immediately after to save doing the lookup twice.
  ///
  /// For example:
  /// ~~~
  /// if (index->exists("MyValue")) return getValue();
  /// ~~~
  VRAnyCoreType getValue() const {

    if (_lastDatum == _theIndex.end()) {
      VRERROR("Bad key access in data index.",
              "The no-arg version of getValue() must be preceded by a call to exists().");
    } else {
      return _lastDatum->second->getValue();
    }
  }


  /// \brief Returns an index value, with a default value.
  ///
  /// Returns the value of the data field stored under key, offers the
  /// opportunity to provide a default value if the given name is not
  /// found.  The cast rules for getValue() apply here, and you will
  /// sometimes need to cast the default argument, too.
  ///
  /// \param key The name of the data field to retrieve.  The name may include
  /// namespaces, separated by slashes.  Valid names are strings like "cora",
  /// "nora/cora", and "/flora/nora/cora".  The last name is "fully qualified,
  /// which means it begins with a slash.  In this case, the function only
  /// returns a value if that exact name appears in the data index and the
  /// nameSpace argument is ignored, if it is provided.
  /// \param defaultVal The default value to be returned if the given key is
  /// not found.  This must be an unambiguous match to the return value, use
  /// a cast if there is any ambiguity.
  /// \param nameSpace The container in which to look for the key.
  /// \param inherit Set to false if you only want values from the specified
  /// container/namespace and not inherit from any namespace above.
  template <typename T>
  T getValueWithDefault(const std::string &key,
                        const T &defaultVal,
                        const std::string nameSpace = "",
                        const bool inherit = true) const {
    VRDataMap::iterator p =
      const_cast<VRDataIndex*>(this)->_getEntry(key, nameSpace, inherit);

    if (p == _theIndex.end()) {
      return defaultVal;
    } else {
      return p->second->getValue();
    }
  }

  /// \brief Returns the type of the specified value.
  ///
  /// The name might point to a data value, or a container, so the type
  /// returned might indicate that the name represents a container
  /// (VRCORETYPE_CONTAINER).
  ///
  /// \param key Can be a full key (namespace + data field name) or just a
  /// data name.
  /// \param nameSpace The (optional) namespace in which to look for key.
  /// \param inherit If this is false, the function will only succeed
  /// if the specified key is found within the given nameSpace.
  /// Otherwise, it will search in parent nameSpaces for the matching
  /// key.  (Default: true)
  VRCORETYPE_ID getType(const std::string &key,
                        const std::string nameSpace = "",
                        const bool inherit = true) const {
    return _getDatum(key, nameSpace, inherit)->getType();
  }

  /// \brief Returns the type of the last searched value.
  ///
  /// This is comparable to the argument-free getValue().  Use it to
  /// get the type of whatever you looked for last.
  VRCORETYPE_ID getType() {
    if (_lastDatum == _theIndex.end()) {
      VRERRORNOADV("Bad key access in data index.");
    } else {
      return _lastDatum->second->getType();
    }
  }

  /// \brief Returns the type of the specified name, formatted as a string.
  ///
  /// \param key Can be a full name (namespace + data field name) or just a
  /// namespace.
  /// \param nameSpace The (optional) container in which to look for key.
  /// \param inherit If this is false, the function will only succeed
  /// if the specified key is found within the given nameSpace.
  /// Otherwise, it will search in parent nameSpaces for the matching
  /// key.  (Default: true)
  std::string getTypeString(const std::string key,
                            const std::string nameSpace = "",
                            const bool inherit = true) const {
    return _getDatum(key, nameSpace, inherit)->getDescription();
  }

  /// \brief Returns the description of the last searched value.
  ///
  /// This is comparable to the argument-free getValue().  Use it to
  /// get the description of whatever you looked for last.
  std::string getTypeString() {
    if (_lastDatum == _theIndex.end()) {
      VRERRORNOADV("Bad key access in data index.");
    } else {
      return _lastDatum->second->getDescription();
    }
  };

  /// \brief Returns the full name of the specified value.
  ///
  /// Returns the fully qualified name of the specified value, starting with
  /// the root namespace.
  /// \param key Can be a full key (namespace + data field name) or just a
  /// data name.
  /// \param nameSpace The (optional) namespace in which to look for key.
  /// \param inherit If this is false, the function will only succeed
  /// if the specified key is found within the given nameSpace.
  /// Otherwise, it will search in parent nameSpaces for the matching
  /// key.  (Default: true)
  std::string getFullKey(const std::string &key,
                         const std::string nameSpace = "",
                         const bool inherit = true) const;

  /// \brief Returns the full name of the last searched value.
  ///
  /// This is comparable to the argument-free getValue().  Use it to
  /// get the full name of whatever you looked for last.
  std::string getFullKey() {
    if (_lastDatum == _theIndex.end()) {
      VRERRORNOADV("Bad key access in data index.");
    } else {
      return _lastDatum->first;
    }
  };

  /// \brief Selects the first value found with the given attribute.
  ///
  /// Searches the current nameSpace/scope, then tries all the namespaces
  /// containing that current one, in order, going up.  Returns the first
  /// object found, or NULL if not found.
  ///
  /// \param attrName The name of an attribute to check.
  /// \param attrVal The value of that attribute.
  /// \param nameSpace The container to search; defaults to the global namespace.
  std::string getByAttribute(const std::string &attrName,
                             const std::string &attrVal,
                             const std::string nameSpace = "") const;


  /// \brief Returns true if the specified name exists in the index.
  /// \param key Can be a full key (namespace + data field name) or just a
  /// data name.
  /// \param nameSpace The (optional) namespace in which to look for key.
  /// \param inherit If this is false, the function will only succeed
  /// if the specified key is found within the given nameSpace.
  /// Otherwise, it will search in parent nameSpaces for the matching
  /// key.  (Default: true)
  bool exists(const std::string &key,
              const std::string nameSpace = "",
              const bool inherit = true) const {
	  return const_cast<VRDataIndex*>(this)->_getEntry(key, nameSpace, inherit) != _theIndex.end();
  }

  ///@}

  ///@{
  /// \name Select values within the index.
  ///
  /// The selection methods return VRContainer, a list of names of objects
  /// satisfying the given criterion.  You can select by name, attribute, or
  /// data type.

  /// \brief Returns a list of names of objects with the given attribute.
  ///
  /// Returns a list of full names for index entries that satisfy
  /// attrName=attrVal.  Attributes should not be used to store major
  /// datatypes, as this is the job of the rest of the data index, but they
  /// can be a useful way to tag data for easy retrieval:
  ///   ~~~
  ///   // select all VRDataIndex entries that were tagged in the
  ///   // windownodes.xml file with an attribute nodeType="WindowNode"
  ///   VRDataIndex index();
  ///   index.setName("TagExample");
  ///   index.processXMLFile("windownodes.xml");
  ///   std::list<std::string> myNodes;
  ///   myNodes = index.selectByAttribute("nodeType", "WindowNode");
  ///   // now, loop through myNodes and do something...
  ///   ~~~
  /// \param attrName The name of an attribute to check.
  /// \param attrVal The value of that attribute.  Use "*" for a wildcard.
  /// \param nameSpace The container to search; defaults to the global namespace.
  /// \param childOnly When true, selects only values that are the direct children
  ///        of the given nameSpace, ignores grandchildren, great-grandchildren, etc.
  ///        If false, collects everything with the attribute, at whatever level
  ///        inside the given nameSpace.
  VRContainer selectByAttribute(const std::string &attrName,
                                const std::string &attrVal,
                                const std::string nameSpace = "",
                                const bool childOnly = false) const;

  /// \brief Returns objects with the given type.
  ///
  /// Returns a list of names of objects with the given type.
  ///
  /// \param typeID The ID to select by.
  /// \param nameSpace Search within this namespace.
  /// \param childOnly Only return names that are the immediate child of the
  ///        given namespace.  That is, ignore grandchildren and
  ///        great-grandchildren, etc.
  VRContainer selectByType(const VRCORETYPE_ID &typeID,
                           const std::string nameSpace = "",
                           const bool childOnly = false) const;

  /// \brief Returns objects with the given name.
  ///
  /// Returns a list of names of objects with the given name.
  /// \param inName The name can be of any length, and may include
  ///        namespaces (scopes).
  /// \param nameSpace An optional namespace that will be prepended to the input
  ///        name before testing.
  VRContainer selectByKey(const std::string &inName,
                          const std::string nameSpace = "") const;

  ///@}

  ///@{
  /// \name Serialization for file and network use.
  ///
  /// These methods are for converting a data index or a part of a data index,
  /// to a byte stream suitable for sending over a network, or storing as a
  /// file.  You can serialize an index or read in serialized data via
  /// addSerializedValue(), or processXMLFile().
  ///
  /// There is also a class constructor that takes a serialized set
  /// of values.  Many users will find this is all that is necessary to read
  /// the data they need read.
  //
  /// \brief Returns an XML formatted representation of the named element.
  ///
  /// \param key The name of the value to be serialized.  This can be
  /// aprimitive value or a container.
  /// \param nameSpace Where to find the value to be serialized.
  /// \param inherit Do we confine our search for key to the current nameSpace?
  std::string serialize(const std::string key,
                        const std::string nameSpace = "",
                        const bool inherit = true) const;

  /// \brief Returns an XML formatted representation of the entire index.
  ///
  /// The root element of the XML output is the name of the index, controlled
  /// with setName().  The special constructor that starts with a serialized
  /// string can use this output to recreate an index.
  ///
  /// Note: Use the copy constructor if you want to create a copy of an index.
  /// The serialize method does not record any links in the index.
  std::string serialize() const;

  /// Incorporates a serialized bit of data into the data index within
  /// the specified container.
  /// \param serializedData XML-formatted data, such as is output from
  ///  the serialize() method.
  /// \param nameSpace Defaults to the root-level container (the global
  /// namespace).
  /// \param expand Set to false if you don't want linkNode attributes
  /// to be expanded using linkNodes().  This is a debugging aid.
  std::string addSerializedValue(const std::string serializedData,
                                 const std::string nameSpace = "",
                                 const bool expand = true);

  /// \brief Add the contents of a given XML file into the index.
  ///
  /// \param fileName Path and name of xml-formatted file
  /// \param nameSpace Defaults to the global namespace.
  bool processXMLFile(const std::string fileName,
                      const std::string nameSpace = rootNameSpace);

  /// \brief Process a command line argument.
  ///
  /// Use this one at the start of a program.  It reads a file if
  /// there's a file, and reads from a pipe if there's a pipe.
  bool processXML(const std::string arg);

  ///@}

  ///@{
  /// \name Attribute management
  ///
  /// Values in the data index can be assigned "attributes", arbitrary named
  /// string values that can be used to describe a piece of data.  The data
  /// index uses some attributes for its own use, `type`, used to specify a
  /// data type in the serialized XML format, and `linkNode` and `linkContent`
  /// to control linking between names.  These attributes appear in input XML
  /// files, but do not appear as attributes in a data index.  The `separator`
  /// attribute does appear in the data index, and is used for the printed
  /// form of an integer, float, or string array.  Don't use these names, but
  /// feel free to make up any other names you like for your attributes.
  ///
  /// Attributes are stored as simple key=value pairs, and the values are all
  /// std::string.  There are some methods for managing attributes.


  /// Returns the value of a specific attribute.
  ///
  /// \param fullKey A fully-qualified key name.  If the key is at the root
  /// level, the leading slash can be omitted.  A key at any other scope must
  /// contain the whole thing.  See getName() if you need to specify the
  /// container name, too.
  ///
  /// \param attributeName The name of an attribute of that value.
  std::string getAttributeValue(const std::string &fullKey,
                                const std::string &attributeName) const;

  /// \brief Check if a data value has an attribute.
  ///
  /// Returns true if the named data index entry has the specified attribute defined.
  ///
  /// \param fullKey A fully-qualified key name.  If the key is at the root
  /// level, the leading slash can be omitted.  A key at any other namespace must
  /// contain the whole thing.  See getName() if you need to specify the
  /// container name, too.
  ///
  /// \param attributeName The name of the attribute to check.
  ///
  bool hasAttribute(const std::string &fullKey,
                    const std::string &attributeName) const;

  /// Sets a specific attribute (tag) for a data index entry.
  ///
  /// \param fullKey A fully-qualified key name.  If the key is at the root
  /// level, the leading slash can be omitted.  A key at any other namespace must
  /// contain the whole thing.  See getName() if you need to specify the
  /// container name, too.
  ///
  /// \param attributeName The name of an attribute of that value.
  ///
  /// \param attributeValue The value to set the attribute to.
  void setAttributeValue(const std::string &fullKey,
                         const std::string &attributeName,
                         const std::string &attributeValue);

  ///@}


  ///@{
  /// \name Saving and restoring state.
  ///
  /// The data index has a "state" that can be saved.  If you "push" the state
  /// onto the stack, it can be restored later with a "pop", which will delete
  /// any of the changes made to the index after the push.  (Though it cannot
  /// restore data values deleted after a push.)

  /// \brief Saves the current index state.
  ///
  /// The data index has a state that can be pushed and popped.  All the
  /// changes to the index made after a pushState() can be rolled back by
  /// calling popState().  This works by pushing and popping values for each of
  /// the VRDatum objects represented in the index.  Values that were added to
  /// the index after a push will be deleted on a pop, but the system cannot
  /// restore deleted values.
  ///
  /// The stack is recorded at the level of the data values, so there is
  /// currently no way to record a name of a stack entry, or even to count
  /// them.  Too many pops will delete entries from your data index, so it's
  /// up to you to be careful about keeping it symmetric.
  void pushState();

  /// \brief Restores the index state to the last push.
  ///
  /// Replaces the current contents of the data index with the data saved via
  /// an earlier call to pushState().
  void popState();

  ///@}


  ///@{
  /// \name Link operations.
  ///
  /// The data index allows two names to point to the *same* value.  This is
  /// established via one of two different attributes in the input XML.  A
  /// `linkNode` attribute will cause the element name to link to the value
  /// indicated by the attribute name.  The `linkContent` attribute will
  /// *replace* the element name with links to the contents of the element
  /// matching the attribute name.
  ///
  /// Example:
  ///
  /// The following will result in two names (/A/B and /A/C/D) pointing to the
  /// *same* value of 6.
  ///
  ///   ~~~
  ///   <A>
  ///     <B>6</B>
  ///     <C>
  ///       <D linkNode="/A/C"/>
  ///     </C>
  ///   </A>
  ///   ~~~
  ///
  /// The following input XML will result in the node /a/e containing links to
  /// the contents of /a/b.  That is, there will be a /a/e/c value linked to
  /// /a/b/c and an /a/e/d value linked to /a/b/d.
  ///
  ///   ~~~
  ///   <a>
  ///     <b>
  ///       <c>3</c>
  ///       <d>2.7</d>
  ///     </b>
  ///     <e>
  ///       <f linkContent="/a/b"/>
  ///     </e>
  ///   </a>
  ///   ~~~


  /// \brief Links one name to another.
  ///
  /// This implements a 'link' operation that makes two names point to the same
  /// value.
  ///
  /// \param fullSourceName The name of the data value to be linked.
  /// \param fullTargetName The name to link it to.
  /// \param depthLimit A recursion limit to guard against circular links.
  ///
  /// This functionality is available in the XML config files, through a
  /// 'linkNode' element.  Use it like this:
  ///    ~~~
  ///    <targetname linkNode="sourcename"/>
  ///    ~~~
  /// This will create an entry in the resulting data index with the
  /// targetname linked to the *same* data object as sourcename.
  ///
  /// Note that linking *containers* probably does not work the way you might
  /// imagine.  Linking containers results in two containers containing
  /// values linked to each other.  Unlike the other linked data types, they
  /// will not be the same container.  That is, adding a value to one of two
  /// linked containers will not add it to the other.
  bool linkNode(const std::string &fullSourceName,
                const std::string &fullTargetName,
                int depthLimit = 0);

  ///@}

  ///@{
  /// \name Utility methods.
  ///
  /// There are several methods that belong to no category beyond simply being
  /// useful to developers using the data index.


  // Some constants that may be useful to users of this API.
  static std::string rootNameSpace;

  /// \brief Return a valid namespace.
  ///
  /// Checks to make sure the given string is a name in the data index, and
  /// that it begins and ends with a `/`, the two qualities that define
  /// "valid" for namespaces.  The slashes will be added if they are not
  /// present, but if there is no match for the namespace in the index, the
  /// function throws an exception.
  ///
  /// \param nameSpace An input string.  The method will check to see if a
  ///                  container of that name is in the data index, and return
  ///                  it with a `/` on the beginning and end if it is.  An empty
  ///                  input string will return `/`, the root namespace.
  std::string validateNameSpace(const std::string &nameSpace) const;

  /// \brief Find the value of an environment variable.
  ///
  /// Dereferences an environment variable in a character string.  The
  /// variable should be enclosed in brackets (`{}`) and preceded by a dollar
  /// sign (`$`), like this: `${MYVARIABLE}/my/file/name`.
  ///
  /// \param fileName A character string that potentially contains one or more
  ///                 environment variables to be translated.
  static std::string dereferenceEnvVars(const std::string &fileName);

  /// \brief Uses names to determine if one value is the child of another.
  ///
  /// Compares names to see if the second is the child of the first.
  ///
  /// \return If they are not related, returns -1.  If they are equal, returns
  /// 0.  If childName is the direct child of parentName, returns 1.  If the
  /// grandchild, returns 2, great-grandchild, 3, etc.
  static int isChild(const std::string &parentName, const std::string &childName);

  /// \brief Returns a printable representation of the data index.
  ///
  /// Returns a string nicely formatted to print the hierarchical structure of
  /// the index or some container in it.
  ///
  /// \param itemName The container to print.  Defaults to the root-level
  ///                 namespace to print the entire index.
  /// \param lim The maximum number of characters to output on each line.  If
  ///            exceeded, the text will be truncated.
  std::string printStructure(const std::string itemName = "/",
                             const size_t lim = 80) const;

  /// \brief Returns a list of all the fully-qualified names in the index.
  ///
  /// \return Note this really is a list of all the strings, not a
  /// VRContainer.  That is, they may seem like the same thing, but there is
  /// no container in the index that contains these names.  If you want a list
  /// of the names in some container, use getValue().
  std::list<std::string> findAllNames() const;

   /// \brief Does the index have any entries?
  ///
  /// \return A boolean value, true if empty.
  bool empty() { return _theIndex.empty(); };
  ///@}


private:
  static VRDatumFactory _factory;
  static VRDatumFactory _initializeFactory();

  typedef std::map<std::string, VRDatumPtr> VRDataMap;
  // Aspirational:
  //typedef std::map<std::string, std::vector<VRDatumPtr> > VRDataMap;
  VRDataMap _theIndex;
  VRDataMap::iterator _lastDatum;

  // This is the name of the data index itself.
  std::string _indexName;

  // If this is 1, new values will overwrite old ones.  For -1, new
  // values will just bounce off.  And zero will cause an exception if
  // an overwrite is attempted.  Except containers, who are always
  // happy to receive new values and add to their existing list.  The
  // default class constructor allows overwrites.
  int _overwrite;

  // Tries to guess a data type from the ASCII representation.
  VRCORETYPE_ID _inferType(const std::string &valueString) const;

  // These functions read an XML-encoded string and produce the value
  // implied.  There is no deserializeContainer, since that's what
  // _walkXML does.
  VRInt _deserializeInt(const char* valueString);
  VRFloat _deserializeFloat(const char* valueString);
  VRString _deserializeString(const char* valueString);
  VRIntArray _deserializeIntArray(const char* valueString, const char separator);
  VRFloatArray _deserializeFloatArray(const char* valueString,
                                      const char separator);
  VRStringArray _deserializeStringArray(const char* valueString,
                                        const char separator);

  // Serializes the given VRDatum object, using the given name.
  std::string _serialize(const std::string &name, const VRDatumPtr &pdata) const;


  // Just a utility to return the tail end of the fully qualified name.
  // i.e. trimName("cora/flora", "/bob/nora") is "flora".  This does not
  // look in the index at all, just manipulates strings.
  static std::string _getTrimName(const std::string &key,
                                 const std::string nameSpace = "");

  // Another utility, meant to pull a name apart on the slashes.
  static std::vector<std::string> _explodeName(const std::string &fullName);

  // Returns the namespace, derived from a long, fully-qualified, name.
  static std::string _getNameSpace(const std::string &fullName);

  // Start from the root node of an XML document and process the
  // results into entries in the data index.
  std::string _walkXML(element* node, std::string nameSpace);
  // A functional part of the walkXML apparatus.
  std::string _processValue(const std::string &name,
                           VRCORETYPE_ID &type,
                           const char* valueString,
                           const char separator);

  /// \brief Finds an entry in the data index.
  ///
  /// Uses a name and namespace to find an entry in the data index.  Note that
  /// the name might be in a senior namespace to the one specified.  That is, if
  /// you have a value called flora, that exists inside a container called a,
  /// but also in a subsidiary container called b, then, well, here's the
  /// example:
  ///   ~~~
  ///   /a/flora = 6
  ///   /a/b/flora = 7
  ///   ~~~
  /// If the namespace is /a, the value of flora is 6, but if the
  /// namespace is /a/b, flora is 7.
  ///
  /// Returns an iterator pointing to an entry in the DataMap.  The
  /// return value is a pair<string, VRDatumPtr>, so it->first is the
  /// name and it->second is the datum object.
  ///
  /// Note that this does not return a const iterator, so runs afoul
  /// of C++ const-ness rules when it's used inside a const method
  /// like exists().  You can get around this with a cast, spelled
  /// like this:
  ///  ~~~
  ///  const_cast<VRDataIndex*>(this)->_getEntry(...)
  ///  ~~~
  /// \param key Can be a full key (namespace + data field name) or just a
  /// data name.
  /// \param nameSpace The (optional) namespace in which to look for key.
  /// \param inherit If this is false, the function will only succeed
  /// if the specified key is found within the given nameSpace.
  /// Otherwise, it will search in parent nameSpaces for the matching
  /// key.  (Default: true)
  VRDataMap::iterator _getEntry(const std::string &key,
                                const std::string nameSpace = "",
                                const bool inherit = true);

  // Returns a pointer to the value with a given name (and namespace)
  /// \param key Can be a full key (namespace + data field name) or just a
  /// data name.
  /// \param nameSpace The (optional) namespace in which to look for key.
  /// \param inherit If this is false, the function will only succeed
  /// if the specified key is found within the given nameSpace.
  /// Otherwise, it will search in parent nameSpaces for the matching
  /// key.  (Default: true)
  VRDatumPtr _getDatum(const std::string &key,
                       const std::string nameSpace = "",
                       const bool inherit = true);

  // Returns a pointer to the value with a given name (and namespace)
  /// \param key Can be a full key (namespace + data field name) or just a
  /// data name.
  /// \param nameSpace The (optional) namespace in which to look for key.
  /// \param inherit If this is false, the function will only succeed
  /// if the specified key is found within the given nameSpace.
  /// Otherwise, it will search in parent nameSpaces for the matching
  /// key.  (Default: true)
  const VRDatumPtr _getDatum(const std::string &key,
                             const std::string nameSpace = "",
                             const bool inherit = true) const;

  // These are specialized set methods.  They seem a little unhip, but
  // it's because I find this easier than remembering how to spell the
  // pointers and casts.
  void _setValueSpecialized(VRDatumPtr p, VRInt value) {
    p.intVal()->setValue(value);
  }

  void _setValueSpecialized(VRDatumPtr p, VRFloat value) {
    p.floatVal()->setValue(value);
  }

  void _setValueSpecialized(VRDatumPtr p, VRString value) {
    p.stringVal()->setValue(value);
  }

  void _setValueSpecialized(VRDatumPtr p, VRIntArray value) {
    p.intArrayVal()->setValue(value);
  }

  void _setValueSpecialized(VRDatumPtr p, VRFloatArray value) {
    p.floatArrayVal()->setValue(value);
  }

  void _setValueSpecialized(VRDatumPtr p, VRStringArray value) {
    p.stringArrayVal()->setValue(value);
  }

  template <typename T, const VRCORETYPE_ID TID>
  std::string addDataSpecialized(const std::string key, T value) {

    // All names must be in some namespace. If there is no namespace,
    // put this into the root namespace.
    std::string fixedValName = key;
    if (key[0] != '/') fixedValName = std::string("/") + key;

    std::pair<VRDataMap::iterator, bool>res =
      _theIndex.insert(VRDataMap::value_type(fixedValName, (VRDatumPtr)NULL));

    // Was it already used?
    if (res.second) {

      VRDatumPtr obj = _factory.CreateVRDatum(TID, &value);
      res.first->second = obj;

      // Add this value to the parent container, if any.
      VRContainer cValue;
      cValue.push_back(_explodeName(fixedValName).back());
      std::string ns = _getNameSpace(fixedValName);
      // The parent container is the namespace minus the trailing /.
      if (ns.compare("/") != 0) addData(ns.substr(0, ns.size() - 1), cValue);

    } else {

      // Entry already exists. Decide whether to modify or throw an exception.
      if (_overwrite > 0) {

        _setValueSpecialized(res.first->second, (T)value);

      } else if (_overwrite == 0) {

        throw std::runtime_error(std::string("overwriting values not allowed"));
      }
    }

    return fixedValName;
  }

  // This does a global resolution of all linkNodes definitions.  It
  // is typically done as part of the process of incorporating some
  // XML.
  bool _linkNodes();

  // This is a roughly similar concept except that it copies the
  // contents of one namespace into another.  Looks for container
  // nodes with a 'linkContent' attribute and inserts into it links
  // from the specified container.
  bool _linkContent();

  // We need this to keep track of links so we can make a deep copy that
  // includes any links that might exist.
  std::map<std::string, std::string> _linkRegister;

  // If this is false, we don't need to do linkNodes() or linkContent().
  bool _linkNeeded;

  friend std::ostream & operator<<(std::ostream &os, const VRDataIndex& di) {
    return os << di.printStructure();
  }
};

/// \brief A class to hold an arbitrary event.
///
/// An event in MinVR is just a VRDataIndex object.
typedef VRDataIndex VRRawEvent;




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
//        - Need a getName(key, nameSpace) [DONE]
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
//     format for serialization. [DONE]
//
//   - The mvrTypeMap is clunky and is not attached to the VRDatum
//     description field.  The mapping between type ID and description
//     should appear only once, somewhere. [DONE]
//
//   - Improve (and test) adding values to containers. [DONE]
//
//   - The walkXML should look out for linkNode attributes, so we can skip
//     the linkNodes() and linkContent() steps of deserialization. [DONE]
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
//   - getDatum() should be private.  Add attribute handling methods to the
//     data index, and the VRDatum type can be completely hidden. [DONE]
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
