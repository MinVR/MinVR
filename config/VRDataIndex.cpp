#include "VRDataIndex.h"

std::string VRDataIndex::rootNameSpace = "/";

// Step 7 of the specialization instructions (in VRDatum.h) is to
// add an entry here to register the new data type.
VRDataIndex::VRDataIndex()  : overwrite(1) {
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
    mVRTypeMap[std::string(m->VRTypeMap[i].first)] = m->VRTypeMap[i].second;
  }
  delete m;
}

std::string VRDataIndex::serialize(const std::string trimName,
                                   VRDatumPtr pdata ) {
  // If this is not a container, just spell out the XML with the serialized
  // data inside.
  if (pdata->getType() != VRCORETYPE_CONTAINER) {

    return ("<" + trimName +
            " type=\"" + pdata->getDescription() + "\"" +
            pdata->getAttrListAsString() + ">" +
            pdata->getValueAsString() +
            "</" + trimName + ">");

  } else {
    // If this is a container...

    std::string serialized;
    //                      ... open the XML tag, with the type ...
    serialized = "<" + trimName +
      " type=\"" + pdata->getDescription() + "\"" +
      pdata->getAttrListAsString() + ">";

    // ... loop through the children (recursively) ...
    VRContainer nameList = pdata->getValue();
    for (VRContainer::iterator lt = nameList.begin();
         lt != nameList.end(); lt++) {

      // ... recurse, and get the serialization of the member data value.
      serialized += serialize(*lt);
    };

    serialized += "</" + trimName + ">";
    return serialized;
  }
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

VRIntArray VRDataIndex::deserializeIntArray(const char* valueString,
                                            const char separator) {

  VRIntArray vVal;

  // Separate the name space into its constituent elements.
  std::string elem;
  VRInt iVal;
  std::stringstream ss(valueString);
  while (std::getline(ss, elem, separator)) {

    sscanf(elem.c_str(), "%d", &iVal);
    vVal.push_back(iVal);
  }

  return vVal;
}

VRDoubleArray VRDataIndex::deserializeDoubleArray(const char* valueString,
                                                  const char separator) {

  VRDoubleArray vVal;

  // Separate the name space into its constituent elements.
  std::string elem;
  VRDouble fVal;
  std::stringstream ss(valueString);
  while (std::getline(ss, elem, separator)) {

    sscanf(elem.c_str(), "%lf", &fVal);
    vVal.push_back(fVal);
  }

  return vVal;
}

VRStringArray VRDataIndex::deserializeStringArray(const char* valueString,
                                                  const char separator) {

  VRStringArray vVal;

  // Separate the name space into its constituent elements.
  VRString elem;
  std::stringstream ss(valueString);
  while (std::getline(ss, elem, separator)) {

    vVal.push_back(elem);
  }

  return vVal;
}

std::string VRDataIndex::processValue(const std::string name,
                                      VRCORETYPE_ID type,
                                      const char* valueString,
                                      const char separator) {

  std::string out;
  /// Step 9 of adding a data type is adding entries to this switch.
  /// And the corresponding deserialize*() method.
  switch (type) {
  case VRCORETYPE_INT:
    out = addData(name, (VRInt)deserializeInt(valueString));
    break;

  case VRCORETYPE_DOUBLE:
    out = addData(name, (VRDouble)deserializeDouble(valueString));
    break;

  case VRCORETYPE_STRING:
    out = addData(name, (VRString)deserializeString(valueString));
    break;

  case VRCORETYPE_INTARRAY:
    out = addData(name, (VRIntArray)deserializeIntArray(valueString, separator));
    break;

  case VRCORETYPE_DOUBLEARRAY:
    out = addData(name, (VRDoubleArray)deserializeDoubleArray(valueString, separator));
    break;

  case VRCORETYPE_STRINGARRAY:
    out = addData(name, (VRStringArray)deserializeStringArray(valueString, separator));
    break;

  case VRCORETYPE_CONTAINER:
    {
      // Check to see if this is just white space. If so, ignore. If
      // not, throw an exception because we don't know what to do.
      std::string stVal = std::string(valueString);
      std::string::iterator end_pos = std::remove(stVal.begin(), stVal.end(), ' ');
      stVal.erase(end_pos, stVal.end());

      if (stVal.size() > 0) {
        throw std::runtime_error(std::string("empty containers not allowed"));
      }
      out = "";
      break;
    }
  case VRCORETYPE_NONE:
    {
      out = "";
      break;
    }	     
  }
  return out;
}

// This seems to read containers twice.  Do both instances wind up in memory?
std::string VRDataIndex::walkXML(element* node, std::string nameSpace) {
  // This method will return the name of the last top-level element in
  // the input XML.  This aspect of its operation could probably use
  // more testing.

  std::string qualifiedName;
  VRContainer childNames;
  std::string out;

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

        if (node->get_attribute("type") == NULL) {

          typeId = inferType(std::string(node->get_value()));
        } else { // what does map return if no match?
          typeId = mVRTypeMap[std::string(node->get_attribute("type")->get_value())];
        }

        // Need a check for typeId == 0

        char separator;
        if (node->get_attribute("separator") == NULL) {
          separator = MINVRSEPARATOR;
        } else {
          separator = *(node->get_attribute("separator")->get_value());
        }
        
        out = processValue(qualifiedName,
                           typeId,
                           node->get_value(),
                           separator);

        // Any attributes to add to the list?
        VRDatum::VRAttributeList al = node->get_attribute_map();
        if (al.find("type") != al.end()) al.erase(al.find("type"));
        if (al.size() > 0) {
          getDatum(out)->setAttributeList(al);
        }
      }
    }

    // Pick the next child.
    element* child = node->get_next_child();
    if (child == NULL) {

      // If this is a non-empty container that is not named XML_DOC,
      // add it to the index.
      if (childNames.size() > 0 && strcmp(node->get_name(), "XML_DOC")) {

        out = addData(qualifiedName, childNames);

        // Pick up any attributes.
        VRDatum::VRAttributeList al = node->get_attribute_map();
        if (al.find("type") != al.end()) al.erase(al.find("type"));
        if (al.size() > 0) {
          getDatum(out)->setAttributeList(al);
        }
      }
      return out;
    }

    // Collect a child name on the container's child name list.
    childNames.push_back(qualifiedName + "/" + child->get_name());

    // And go walk its tree.
    walkXML(child, qualifiedName + "/");
  }
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

  firstChar = valueString.find(MINVRSEPARATOR);
  if (firstChar != std::string::npos) {
    // It's an array...

    // Test for int
    conInt = strtol(valueString.substr(0, firstChar).c_str(), &p, 10);
    if (!*p) return VRCORETYPE_INTARRAY;

    conDouble = strtod(valueString.substr(0, firstChar).c_str(), &p);
    if (!*p) return VRCORETYPE_DOUBLEARRAY;
  }    
  
  // Not any of the above?  Probably a string.
  return VRCORETYPE_STRING;
}

bool VRDataIndex::printXML(element* node, std::string prefix) {

  std::string newPrefix = prefix + std::string("| ");

  std::cout << newPrefix << "Node: " << node->get_name() << std::endl;
  if (node->get_value())
    std::cout << newPrefix << "Value: >" << node->get_value() << "<" << std::endl;

  attribute *attr = node->get_next_attribute();
  while (attr != NULL) {
    std::cout << newPrefix << "| | Attribute: " << attr->get_name() << std::endl;
    if (attr->get_value())
      std::cout << newPrefix << "| | Value: " << attr->get_value() << std::endl;

    attr = node->get_next_attribute();
  }

  element *child = node->get_next_child();
  while (child != NULL) {

    printXML(child, newPrefix);

    child = node->get_next_child();
  }
  node->reset_iterators();

  return true;

}




// Just returns a list of the data names. For implementing an 'ls'
// command, or something like it.  If you want a list of names within
// a container (or within a namespace, pretty much the same thing),
// just use getValue().
std::list<std::string> VRDataIndex::getNames() {
  std::list<std::string> outList;
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); it++) {
    outList.push_back(it->first);
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

      std::vector<std::string> names(&elems[0], &elems[0] + N);
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
VRDatumPtr VRDataIndex::getDatum(const std::string valName) {
  VRDataMap::const_iterator it = mindex.find(valName);
  if (it == mindex.end()) {
    throw std::runtime_error(std::string("never heard of ") + valName);
  } else {
    return it->second;
  }
}

VRDatumPtr VRDataIndex::getDatum(const std::string valName,
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

std::string VRDataIndex::serialize(const std::string valName) {

  VRDataMap::iterator it = getEntry(valName, "");

  if (it != mindex.end()) {

    return serialize(getTrimName(it->first), it->second);

  } else {

    throw std::runtime_error(std::string("never heard of ") + valName);

  }
}

std::string VRDataIndex::serialize(const std::string valName,
                                   const std::string nameSpace) {

  VRDataMap::iterator it = getEntry(valName, nameSpace);

  if (it != mindex.end()) {

    return serialize(getTrimName(it->first), it->second);

  } else {

    throw std::runtime_error(std::string("never heard of ") + valName + std::string(" in the namespace: ") + nameSpace);

  }
}

// an int should be <nWindows type="int">6</nWindows>
std::string VRDataIndex::addSerializedValue(const std::string serializedData) {

  return addSerializedValue(serializedData, std::string(""));
}

std::string VRDataIndex::addSerializedValue(const std::string serializedData,
                                           const std::string nameSpace) {

  Cxml *xml = new Cxml();
  xml->parse_string((char*)serializedData.c_str());
  element *xml_node = xml->get_root_element();
  element* child = xml_node->get_next_child();
  std::string out;
  
  while (child != NULL) {

#ifdef DEBUG
    printXML(child, validateNameSpace(nameSpace));
#endif
    out = walkXML(child, validateNameSpace(nameSpace));

    child = xml_node->get_next_child();
  }

  delete xml;
  return out;
}

std::string VRDataIndex::dereferenceEnvVars(const std::string fileName) {

  // A little loop to accommodate environment variables in the
  // fileName specification. The variables are assumed to be packaged
  // inside a '${}' combination, e.g. ${MVRHOME}/tests/config/test.xml
  // 
  std::string pathName = fileName;  
  int dollarPos = 0;

  dollarPos = pathName.find_first_of("$", dollarPos);
  while (dollarPos != string::npos) {

    int bracketPos = pathName.find_first_of("}", dollarPos);

    if (bracketPos == string::npos) {
      throw std::runtime_error(std::string("bad environment variable syntax"));
    }

    int bracketLen = 1 + bracketPos - dollarPos;
    std::string envVariable = pathName.substr(dollarPos + 2, bracketLen - 3);

    if (getenv(envVariable.c_str()) == NULL) {
      throw std::runtime_error(std::string("no such environment variable:") +
                               envVariable);
    }
    
    pathName.replace(dollarPos, bracketLen, getenv(envVariable.c_str()));
    dollarPos = pathName.find_first_of("$", dollarPos);

    std::cout << "pathName: " << pathName << std::endl;
  } // End environment variable translation.

  return pathName;
}

  
bool VRDataIndex::processXMLFile(const std::string fileName,
                                 const std::string nameSpace) {

  std::string xml_string = "";
  std::string pathName = dereferenceEnvVars(fileName);
  
  std::cout << "Reading from file = " << pathName << std::endl;
  ifstream file(pathName.c_str());

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

std::string VRDataIndex::addData(const std::string valName, VRInt value) {

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

std::string VRDataIndex::addData(const std::string valName, VRDouble value) {

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

std::string VRDataIndex::addData(const std::string valName, VRString value) {

  // Remove leading spaces.
  int valueBegin = value.find_first_not_of(" \t\n\r");
  if (valueBegin == value.size())
    return NULL; // no content. This should not happen. Or should it?

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

std::string VRDataIndex::addData(const std::string valName,
                                VRIntArray value) {

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


std::string VRDataIndex::addData(const std::string valName,
                                VRDoubleArray value) {

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


std::string VRDataIndex::addData(const std::string valName,
                                 VRStringArray value) {

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

std::string VRDataIndex::addData(const std::string valName,
                                 VRContainer value) {

  // If the container to add to is the root, ignore.
  if (valName.compare("/") == 0)
    throw std::runtime_error(std::string("cannot replace the root namespace"));

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

std::string VRDataIndex::addNameSpace(const std::string valName) {

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
  printStructure("/");
}

void VRDataIndex::printStructure(const std::string itemName) {
  
  // Should sort mindex here.

  int i;
  
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); ++it) {

    bool printMe = true;

    std::vector<std::string> itemElems = explodeName( itemName );
    std::vector<std::string> elems = explodeName( it->first );

    for (i = 1; i < min(itemElems.size(), elems.size()); i++) {
      
      if (itemElems[i].compare(elems[i]) != 0)
        printMe = false;
    }

    if (!printMe) continue;
    
    for (i = 0; i < ((int)elems.size() - 1); i++) std::cout << " | ";
    std::cout << elems.back();

    if (it->second->getType() == VRCORETYPE_CONTAINER) {

      std::cout << std::endl;

    } else {

      std::string out = serialize(elems.back(), it->second);
      if (out.size() > 50) {
        out = out.substr(0,49) + "...";
      }
      std::cout << " = " << out << std::endl;
    }
  }
}


