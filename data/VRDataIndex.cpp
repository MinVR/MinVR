#include "VRDataIndex.h"

#include "Cxml/Cxml.h"
#include "XMLUtils.h"

VRDataIndex VRDataIndex::fromXML(const std::string &xmlString) {
  std::map<std::string, std::string> props;
  std::string dataXML;
  std::string leftover;
  // this will strip the opening <VRDataIndex> and closing </VRDataIndex> out of
  // the xmlString, and return the inner matter in the dataXML variable, which
  // is what the VRDataIndex::addDataFromXML() method expects.
  getXMLField(xmlString, "VRDataIndex", props, dataXML, leftover);
  
  VRDataIndex di;
  di.addDataFromXML(dataXML);
  return di;
}



VRDataIndex::VRDataIndex() : overwrite(1) {
  factory.RegisterVRDatum(VRCORETYPE_INT, CreateVRDatumInt);
  factory.RegisterVRDatum(VRCORETYPE_DOUBLE, CreateVRDatumDouble);
  factory.RegisterVRDatum(VRCORETYPE_STRING, CreateVRDatumString);
  factory.RegisterVRDatum(VRCORETYPE_INTARRAY, CreateVRDatumIntArray);
  factory.RegisterVRDatum(VRCORETYPE_DOUBLEARRAY, CreateVRDatumDoubleArray);
  factory.RegisterVRDatum(VRCORETYPE_STRINGARRAY, CreateVRDatumStringArray);
  factory.RegisterVRDatum(VRCORETYPE_CONTAINER, CreateVRDatumContainer);
  
  // We create a VRDatum object here just to have access to the
  // typemap that is a static member of that class.  Copy it into a
  // map<> for use over here in reading serialized data strings.
  VRDatumInt *m = new VRDatumInt(0);
  for (int i = 0; i < VRCORETYPE_NTYPES; i++) {
    vrTypeMap[std::string(m->VRTypeMap[i].first)] = m->VRTypeMap[i].second;
  }
}


// Just returns a list of the data names. For implementing an 'ls'
// command, or something like it.
std::list<std::string> VRDataIndex::getNames() {
  std::list<std::string> outList;
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); it++) {
    outList.push_back(it->first);
  }
  return outList;
}

//
std::list<std::string> VRDataIndex::getNames(const std::string &containerName) {
  std::list<std::string> outList;
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); it++) {

    if (it->first.compare(0, containerName.size(), containerName) == 0) {
      outList.push_back(it->first);
    }
  }
  return outList;
}

// Breaks up a name into its constituent parts, on the slashes.  Note
// that the first element of the return is blank.  This is on purpose,
// since that is more or less the top-level container's name.  But beware
// that feature when you're using the function.
std::vector<std::string> VRDataIndex::explodeName(const std::string fullName) {

  std::vector<std::string> elems;
  std::string elem;
  std::stringstream ss(fullName);
  while (std::getline(ss, elem, '/')) {
    elems.push_back(elem);
  }

  return elems;
}

// Check a few things:
//   1. Namespace begins with a "/"
//   2. Namespace references an actually existing container.
//   3. Namespace ends with a "/" so valNames can just be appended.
std::string VRDataIndex::validateNameSpace(const std::string nameSpace) {

  std::string out = nameSpace;

  if (out[ 0 ] != '/') out = "/" + nameSpace;
  if (out[ out.size() - 1 ] != '/') out += '/';

  // If out is only one character, it's a '/' and we're done.
  if (out.size() > 1) {

    // Otherwise look for it in the index and throw an error if
    // it isn't there.
    if (mindex.find(out.substr(0, out.size() - 1)) == mindex.end()) {

        throw("Can't find a namespace called " + nameSpace);

    }
  }

  return out;
}

// Returns the container name, derived from a long, fully-qualified, name.
std::string VRDataIndex::getNameSpace(const std::string fullName) {

  std::vector<std::string> elems = explodeName(fullName);
  std::string out;

  if (elems.size() > 1) {

    elems.pop_back();

    for (std::vector<std::string>::iterator it = elems.begin();
         it != elems.end(); ++it) {

      out += *it + std::string("/");
    }

  } else {

    // There is no container beside the root.

    out = "/";
  }

  return out;
}



// Combining the name and the namespace allows the caller to
// 'inherit' values from higher-up namespaces.  Consider this example:
//
//  <stanley>
//    <height>4.5</height>
//    <blanche>
//      <height>3.2</height>
//    </blanche>
//    <stella>
//      <eyes>blue</eyes>
//    </stella>
//   </stanley>
//
//  If the default namespace is /stanley/blanche, and you ask for
//  height, you'll get 3.2, while if the namespace is /stanley/stella,
//  you'll get 4.5, since that value is inherited from the higher-up
//  namespace.
VRDataIndex::VRDataMap::iterator
VRDataIndex::getEntry(const std::string valName,
                      const std::string nameSpace) {

  // If the input valName begins with a "/", it is a fully qualified
  // name already.  That is, it already includes the name space.

  VRDataMap::iterator outIt;

  if (valName[0] == '/') {

    return mindex.find(valName);

  } else {

    // Separate the name space into its constituent elements.
    std::vector<std::string> elems = explodeName(nameSpace);

    // We start from the longest name space and peel off the rightmost
    // element each iteration until we find a match, or not.  This
    // provides for the most local version of valName to prevail.  The
    // last iteration creates an empty testSpace, on purpose, to test
    // the root level nameSpace.
    for (int N = elems.size(); N >= 0; --N) {

      std::vector<std::string> names(&elems[0], &elems[N]);
      std::string testSpace;

      for (std::vector<std::string>::iterator it = names.begin();
           it != names.end(); ++it) {

        testSpace += *it + "/" ;
      }

      outIt = mindex.find(testSpace + valName);
      if (outIt != mindex.end()) {
        return outIt;
      }
    }

    // If we are here, there is no matching name in the index.
    return mindex.end();
  }
}

VRDataIndex::VRDataMap::iterator
VRDataIndex::getEntry(const std::string valName) {

  return getEntry(valName, "");
}

std::string VRDataIndex::getName(const std::string valName,
                                 const std::string nameSpace) {

  VRDataMap::iterator p = getEntry(valName, nameSpace);

  if (p == mindex.end()) {
    return std::string("");
  } else {
    return p->first;
  }
}

// Returns the data object for this name.
VRDatumPtr VRDataIndex::getDatumPtr(const std::string valName) {
  VRDataMap::const_iterator it = mindex.find(valName);
  if (it == mindex.end()) {
    throw std::runtime_error(std::string("never heard of ") + valName);
  } else {
    return it->second;
  }
}

VRDatumPtr VRDataIndex::getDatumPtr(const std::string valName,
                                 const std::string nameSpace) {

  VRDataMap::iterator p = getEntry(valName, nameSpace);

  if (p == mindex.end()) {
    throw std::runtime_error(std::string("never heard of ") + valName + std::string(" in any of the namespaces: ") + nameSpace);
  } else {
    return p->second;
  }
}

std::string VRDataIndex::getTrimName(const std::string valName,
                                     const std::string nameSpace) {

  return getTrimName(validateNameSpace(nameSpace) + valName);
}

std::string VRDataIndex::getTrimName(const std::string valName) {


  // This separates the valName on the slashes and puts the last
  // part of it into trimName.
  std::stringstream ss(valName);
  std::string trimName;
  while (std::getline(ss, trimName, '/')) {};

  return trimName;
}


std::string VRDataIndex::getTypeAsString(const std::string &valName) {
  return ("<" + getTrimName(valName) + " type=\"" +
          getDatumPtr(valName)->getTypeAsString() + "\"/>");
}

std::string VRDataIndex::getTypeAsString(const std::string &valName,
                                         const std::string &nameSpace) {

  return ("<" + getTrimName(valName, nameSpace) + " type=\"" +
          getDatumPtr(valName, nameSpace)->getTypeAsString() + "\"/>");
}


std::string VRDataIndex::toXML() {
  std::string out;
  out = "<VRDataIndex>";
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); ++it) {
    out += dataToXML(it->first);
  }
  out += "</VRDataIndex>";
  return out;
}

std::string VRDataIndex::dataToXML(const std::string valName) {

  VRDataMap::iterator it = getEntry(valName, "");

  if (it != mindex.end()) {

    return dataToXML(getTrimName(it->first), it->second);

  } else {

    throw std::runtime_error(std::string("never heard of ") + valName);

  }
}

std::string VRDataIndex::dataToXML(const std::string valName, const std::string nameSpace) {

  VRDataMap::iterator it = getEntry(valName, nameSpace);

  if (it != mindex.end()) {

    return dataToXML(getTrimName(it->first), it->second);

  } else {

    throw std::runtime_error(std::string("never heard of ") + valName + std::string(" in the namespace: ") + nameSpace);

  }
}

std::string VRDataIndex::dataToXML(const std::string trimName, VRDatumPtr pdata ) {
  // If this is not a container, just spell out the XML with the serialized
  // data inside.
  if (pdata->getType() != VRCORETYPE_CONTAINER) {

    // Tom: Why not open the tag here with <VRDatum rather than <trimName?
    return "<" + trimName + " type=\"" + pdata->getTypeAsString() + "\">" +
      pdata->getValueAsString() + "</" + trimName + ">";

  } else {
    // If this is a container...

    std::string serialized;
    //                      ... open the XML tag, with the type ...
    // Tom: Why not open the tag here with <VRContainer or <VRNamespace rather than <trimName?
    serialized = "<" + trimName + " type=\"" + pdata->getTypeAsString() + "\">";

    // ... loop through the children (recursively) ...
    VRContainer nameList = pdata->getValue();
    for (VRContainer::iterator lt = nameList.begin(); lt != nameList.end(); lt++) {

      // ... recurse, and get the serialization of the member data value.
      serialized += dataToXML(*lt);
    };

    serialized += "</" + trimName + ">";
    return serialized;
  }
}

// an int should be <nWindows type="int">6</nWindows>
bool VRDataIndex::addDataFromXML(const std::string serializedData) {

  return addDataFromXML(serializedData, std::string(""));
}

bool VRDataIndex::addDataFromXML(const std::string serializedData, const std::string nameSpace) {

  Cxml *xml = new Cxml();
  xml->parse_string((char*)serializedData.c_str());
  element *xml_node = xml->get_root_element();
  element* child = xml_node->get_next_child();

  while (child != NULL) {

#ifdef DEBUG
    printXML(child, validateNameSpace(nameSpace));
#endif
    walkXML(child, validateNameSpace(nameSpace));

    child = xml_node->get_next_child();
  }

  delete xml;
  return true;
}

bool VRDataIndex::addDataFromXMLFile(std::string fileName, std::string nameSpace) {

  std::string xml_string="";
  std::cout << "Reading from file = " << fileName << std::endl;
  ifstream file(fileName.c_str());

  if(file.is_open()) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    xml_string = buffer.rdbuf()->str();

    Cxml *xml = new  Cxml();
    xml->parse_string((char*)xml_string.c_str());

    element *xml_node = xml->get_root_element();
    element *child = xml_node->get_next_child();

    while (child != NULL) {

#ifdef DEBUG
      printXML(child, nameSpace);
#endif
      walkXML(child, nameSpace);

      child = xml_node->get_next_child();
    }

    delete xml;

  } else {
    std::cout << "Error opening file " << fileName << std::endl;
  }
  return true;
}


VRInt VRDataIndex::deserializeInt(const char* valueString) {
  int iVal;
  sscanf(valueString, "%d", &iVal);
  
  return iVal;
}

VRDouble VRDataIndex::deserializeDouble(const char* valueString) {
  double fVal;
  sscanf(valueString, "%lf", &fVal);
  
  return fVal;
}

VRString VRDataIndex::deserializeString(const char* valueString) {
  return std::string(valueString);
}

VRIntArray VRDataIndex::deserializeIntArray(const char* valueString) {
  
  VRIntArray vVal;
  
  // Separate the name space into its constituent elements.
  std::string elem;
  int iVal;
  std::stringstream ss(valueString);
  while (std::getline(ss, elem, '@')) {
    
    sscanf(elem.c_str(), "%d", &iVal);
    vVal.push_back(iVal);
  }
  
  return vVal;
}


VRDoubleArray VRDataIndex::deserializeDoubleArray(const char* valueString) {
  
  VRDoubleArray vVal;
  
  // Separate the name space into its constituent elements.
  std::string elem;
  double fVal;
  std::stringstream ss(valueString);
  while (std::getline(ss, elem, '@')) {
    
    sscanf(elem.c_str(), "%lf", &fVal);
    vVal.push_back(fVal);
  }
  
  return vVal;
}


VRStringArray VRDataIndex::deserializeStringArray(const char* valueString) {
  
  VRStringArray vVal;
  
  // Separate the name space into its constituent elements.
  std::string elem;
  std::stringstream ss(valueString);
  while (std::getline(ss, elem, '@')) {
    vVal.push_back(elem);
  }
  
  return vVal;
}



bool VRDataIndex::processValue(const char* name, VRCORETYPE_ID type, const char* valueString) {
  /// Step 9 of adding a data type is adding entries to this switch.
  /// And the corresponding deserialize*() method.
  switch (type) {
    case VRCORETYPE_INT: {
      VRInt i = deserializeInt(valueString);
      addData(name, i);
      break;
    }
    case VRCORETYPE_DOUBLE: {
      VRDouble d = deserializeDouble(valueString);
      addData(name, d);
      break;
    }
    case VRCORETYPE_STRING: {
      VRString s = deserializeString(valueString);
      addData(name, s);
      break;
    }
    case VRCORETYPE_INTARRAY: {
      VRIntArray ia = deserializeIntArray(valueString);
      addData(name, ia);
      break;
    }
    case VRCORETYPE_DOUBLEARRAY: {
      VRDoubleArray da = deserializeDoubleArray(valueString);
      addData(name, da);
      break;
    }
    case VRCORETYPE_STRINGARRAY: {
      VRStringArray sa = deserializeStringArray(valueString);
      addData(name, sa);
      break;
    }
    case VRCORETYPE_CONTAINER: {
      // Check to see if this is just white space. If so, ignore. If
      // not, throw an exception because we don't know what to do.
      std::string stVal = std::string(valueString);
      std::string::iterator end_pos = std::remove(stVal.begin(), stVal.end(), ' ');
      stVal.erase(end_pos, stVal.end());
      
      if (stVal.size() > 0) {
        throw std::runtime_error(std::string("empty containers not allowed"));
      }
      break;
    }
    case VRCORETYPE_NONE: {
      break;
    }	     
  }
  return true;
}



// This function examines a value string and tries to determine what
// type it encodes.  It is used when the 'type=' attribute is missing.
// This is really just part of trying to make the package easy to use
// for configuration files.  For the serialize/deserialize pair, it's
// not an issue.
VRCORETYPE_ID VRDataIndex::inferType(const std::string valueString) {
  /// Step 10 -- Add some functionality to this method to help identify
  /// your new data type.
  
  // Test for int
  char *p;
  int conInt = strtol(valueString.c_str(), &p, 10);
  if (!*p) return VRCORETYPE_INT;
  
  double conDouble = strtod(valueString.c_str(), &p);
  if (!*p) return VRCORETYPE_DOUBLE;
  
  // Is it a container?
  std::size_t firstChar = valueString.find_first_not_of(" \t\r\n");
  if (firstChar != std::string::npos) {
    if (valueString[firstChar] == '<') return VRCORETYPE_CONTAINER;
  }
  
  // Not any of the above?  Probably a string.
  return VRCORETYPE_STRING;
}

// This seems to read containers twice.  Do both instances wind up in memory?
bool VRDataIndex::walkXML(element* node, std::string nameSpace) {
  
  char type[5] = "type";
  
  std::string qualifiedName;
  VRContainer childNames;
  
  qualifiedName = nameSpace + std::string(node->get_name());
  
  // This loops through the node children, if there are any.
  while (true) {
    
    // If there is a value, submit this node to processValue.
    // Container nodes should not be processed this way because they
    // have children, not a value.  Or at least they should not, and
    // the processValue method will throw an exception.
    if (node->get_value() != NULL) {
      
      // Check that the node value isn't just white space or empty.
      std::string valueString = std::string(node->get_value());
      //std::size_t firstChar = valueString.find_first_not_of(" \t\r\n");
      int firstChar = valueString.find_first_not_of(" \t\r\n");
      
      if (firstChar >= 0) {
        
        VRCORETYPE_ID typeId;
        
        if (node->get_attribute(type) == NULL) {
          
          typeId = inferType(std::string(node->get_value()));
        } else { // what does map return if no match?
          typeId = vrTypeMap[std::string(node->get_attribute(type)->get_value())];
        }
        
        // check for typeId == 0
        
        processValue(qualifiedName.c_str(),
                     typeId,
                     node->get_value());
      }
    }
    
    // Pick the next child.
    element* child = node->get_next_child();
    if (child == NULL) {
      
      // If this is a non-empty container that is not named XML_DOC,
      // add it to the index.
      if (childNames.size() > 0 && strcmp(node->get_name(), "XML_DOC")) {
        
        addData(qualifiedName, childNames);
      }
      return true;
    }
    
    // Collect a child name on the container's child name list.
    childNames.push_back(qualifiedName + "/" + child->get_name());
    
    // And go walk its tree.
    walkXML(child, qualifiedName + "/");
  }
}


std::string VRDataIndex::addData(const std::string &valName, VRInt value) {

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    // No? Create it and stick it in index.
    VRDatumPtr obj = factory.CreateVRDatum(VRCORETYPE_INT, &value);
    mindex.insert(VRDataMap::value_type(valName, obj));

    // Add this value to the parent container, if any.
    VRContainer cValue;
    cValue.push_back(valName);
    std::string ns = getNameSpace(valName);
    // The parent container is the namespace minus the trailing /.
    if (ns.compare("/") != 0) addData(ns.substr(0,ns.size()-1), cValue);

  } else {
    // Overwrite value
    if (overwrite > 0) {
      it->second.intVal()->setValue(value);
    } else if (overwrite == 0) {
      throw std::runtime_error(std::string("overwriting values not allowed"));
    }
  }

  return valName;

}

std::string VRDataIndex::addData(const std::string &valName, VRDouble value) {

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    VRDatumPtr obj = factory.CreateVRDatum(VRCORETYPE_DOUBLE, &value);
    mindex.insert(VRDataMap::value_type(valName, obj));

    // Add this value to the parent container, if any.
    VRContainer cValue;
    cValue.push_back(valName);
    std::string ns = getNameSpace(valName);
    // The parent container is the namespace minus the trailing /.
    if (ns.compare("/") != 0) addData(ns.substr(0,ns.size()-1), cValue);

  } else {
    // Overwrite value
    if (overwrite > 0) {
      it->second.doubleVal()->setValue(value);
    } else if (overwrite == 0) {
      throw std::runtime_error(std::string("overwriting values not allowed"));
    }
  }
  return valName;
}

std::string VRDataIndex::addData(const std::string &valName, VRString value) {

  // Remove leading spaces.
  int valueBegin = value.find_first_not_of(" \t\n\r");
  if (valueBegin == value.size())
    return ""; // no content. This should not happen. Or should it?

  // ... and trailing.
  int valueEnd = value.find_last_not_of(" \t\n\r");
  int valueRange = valueEnd - valueBegin + 1;

  std::string trimValue = value.substr(valueBegin, valueRange);

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    VRDatumPtr obj = factory.CreateVRDatum(VRCORETYPE_STRING, &trimValue);
    mindex.insert(VRDataMap::value_type(valName, obj));

    // Add this value to the parent container, if any.
    VRContainer cValue;
    cValue.push_back(valName);
    std::string ns = getNameSpace(valName);
    // The parent container is the namespace minus the trailing /.
    if (ns.compare("/") != 0) addData(ns.substr(0,ns.size()-1), cValue);

  } else {
    // Overwrite value
    if (overwrite > 0) {
      it->second.stringVal()->setValue(trimValue);
    } else if (overwrite == 0) {
      throw std::runtime_error(std::string("overwriting values not allowed"));
    }
  }
  return valName;
}

std::string VRDataIndex::addData(const std::string &valName, VRIntArray value) {

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    // No? Create it and stick it in index.
    VRDatumPtr obj = factory.CreateVRDatum(VRCORETYPE_INTARRAY, &value);
    mindex.insert(VRDataMap::value_type(valName, obj));

    // Add this value to the parent container, if any.
    VRContainer cValue;
    cValue.push_back(valName);
    std::string ns = getNameSpace(valName);
    // The parent container is the namespace minus the trailing /.
    if (ns.compare("/") != 0) addData(ns.substr(0,ns.size()-1), cValue);

  } else {
    // Overwrite value
    if (overwrite > 0) {
      it->second.intArrayVal()->setValue(value);
    } else if (overwrite == 0) {
      throw std::runtime_error(std::string("overwriting values not allowed"));
    }
  }
  return valName;
}


std::string VRDataIndex::addData(const std::string &valName, VRDoubleArray value) {

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    // No? Create it and stick it in index.
    VRDatumPtr obj = factory.CreateVRDatum(VRCORETYPE_DOUBLEARRAY, &value);
    mindex.insert(VRDataMap::value_type(valName, obj));

    // Add this value to the parent container, if any.
    VRContainer cValue;
    cValue.push_back(valName);
    std::string ns = getNameSpace(valName);
    // The parent container is the namespace minus the trailing /.
    if (ns.compare("/") != 0) addData(ns.substr(0,ns.size()-1), cValue);

  } else {
    // Overwrite value
    if (overwrite > 0) {
      it->second.doubleArrayVal()->setValue(value);
    } else if (overwrite == 0) {
      throw std::runtime_error(std::string("overwriting values not allowed"));
    }
  }
  return valName;
}

std::string VRDataIndex::addData(const std::string &valName, VRStringArray value) {

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    // No? Create it and stick it in index.
    VRDatumPtr obj = factory.CreateVRDatum(VRCORETYPE_STRINGARRAY, &value);
    mindex.insert(VRDataMap::value_type(valName, obj));

    // Add this value to the parent container, if any.
    VRContainer cValue;
    cValue.push_back(valName);
    std::string ns = getNameSpace(valName);
    // The parent container is the namespace minus the trailing /.
    if (ns.compare("/") != 0) addData(ns.substr(0,ns.size()-1), cValue);

  } else {
    // Overwrite value
    if (overwrite > 0) {
      it->second.stringArrayVal()->setValue(value);
    } else if (overwrite == 0) {
      throw std::runtime_error(std::string("overwriting values not allowed"));
    }
  }
  return valName;
}


std::string VRDataIndex::addData(const std::string &valName, VRContainer value) {

  // If the container to add to is the root, ignore.
  if (valName.compare("/") == 0) return valName;

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    VRDatumPtr obj = factory.CreateVRDatum(VRCORETYPE_CONTAINER, &value);
    //std::cout << "added " << obj.containerVal()->getDatum() << std::endl;
    mindex.insert(VRDataMap::value_type(valName, obj));

    // Add this value to the parent container, if any.
    VRContainer cValue;
    cValue.push_back(valName);
    std::string ns = getNameSpace(valName);
    // The parent container is the namespace minus the trailing /.
    if (ns.compare("/") != 0) addData(ns.substr(0,ns.size()-1), cValue);

  } else {
    // Add value to existing container.
    it->second.containerVal()->addToValue(value);
  }
  return valName;
}

std::string VRDataIndex::addData(const std::string &valName) {

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    VRContainer v ;
    VRDatumPtr obj = factory.CreateVRDatum(VRCORETYPE_CONTAINER, &v);
    //std::cout << "added " << obj.containerVal()->getDatum() << std::endl;
    mindex.insert(VRDataMap::value_type(valName, obj));

    // Add this value to the parent container, if any.
    VRContainer cValue;
    cValue.push_back(valName);
    std::string ns = getNameSpace(valName);
    // The parent container is the namespace minus the trailing /.
    if (ns.compare("/") != 0) addData(ns.substr(0,ns.size()-1), cValue);

  }
  return valName;
}


void VRDataIndex::printStructure() {

  // Should sort mindex here.

  int i;
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); ++it) {

    std::vector<std::string> elems = explodeName( it->first );

    for (i = 0; i < ((int)elems.size() - 1); i++) std::cout << " | ";
    std::cout << elems.back();

    if (it->second->getType() == VRCORETYPE_CONTAINER) {

      std::cout << std::endl;

    } else {

      std::string out = dataToXML(elems.back(), it->second);
      if (out.size() > 50) {
        out = out.substr(0,49) + "...";
      }
      std::cout << " = " << out << std::endl;
    }
  }
}


