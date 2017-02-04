#include "VRDataIndex.h"

namespace MinVR {
  
std::string VRDataIndex::rootNameSpace = "/";

// Step 7 of the specialization instructions (in VRDatum.h) is to
// add an entry here to register the new data type.
VRDataIndex::VRDataIndex()  : overwrite(1) {
  factory.RegisterVRDatum(VRCORETYPE_INT, CreateVRDatumInt);
  factory.RegisterVRDatum(VRCORETYPE_FLOAT, CreateVRDatumFloat);
  factory.RegisterVRDatum(VRCORETYPE_STRING, CreateVRDatumString);
  factory.RegisterVRDatum(VRCORETYPE_INTARRAY, CreateVRDatumIntArray);
  factory.RegisterVRDatum(VRCORETYPE_FLOATARRAY, CreateVRDatumFloatArray);
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

std::string VRDataIndex::serialize(const std::string name,
                                   VRDatumPtr pdata ) {

  std::string trimName = getTrimName(name);

  // If this is not a container, just spell out the XML with the serialized
  // data inside.
  if (pdata->getType() != VRCORETYPE_CONTAINER) {

    return ("<" + trimName +
            " type=\"" + pdata->getDescription() + "\"" +
            pdata->getAttributeListAsString() + ">" +
            pdata->getValueString() +
            "</" + trimName + ">");

  } else {
    // If this is a container...

    std::string serialized;
    //                      ... open the XML tag, with the type ...
    serialized = "<" + trimName +
      " type=\"" + pdata->getDescription() + "\"" +
      pdata->getAttributeListAsString();

    VRContainer nameList = pdata->getValue();
    if (nameList.empty()) {

      serialized += "/>";

    } else {

      serialized += ">";

      // ... loop through the children (recursively) ...
      for (VRContainer::iterator lt = nameList.begin();
           lt != nameList.end(); lt++) {

        // ... recurse, and get the serialization of the member data value.
        serialized += serialize(name + "/" + *lt);
      };

      serialized += "</" + trimName + ">";

    }
    return serialized;
  }
}

VRInt VRDataIndex::deserializeInt(const char* valueString) {
  int iVal;
  sscanf(valueString, "%d", &iVal);

  return iVal;
}

VRFloat VRDataIndex::deserializeFloat(const char* valueString) {
  float fVal;
  sscanf(valueString, "%f", &fVal);

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

VRFloatArray VRDataIndex::deserializeFloatArray(const char* valueString,
                                                  const char separator) {

  VRFloatArray vVal;

  // Separate the name space into its constituent elements.
  std::string elem;
  VRFloat fVal;
  std::stringstream ss(valueString);
  while (std::getline(ss, elem, separator)) {

    sscanf(elem.c_str(), "%f", &fVal);
    vVal.push_back(fVal);
  }

  return vVal;
}

VRStringArray VRDataIndex::deserializeStringArray(const char* valueString,
                                                  const char separator) {

  VRStringArray vVal;

  // Separate the name space into its constituent elements.
  VRString elem, out = "";
  std::stringstream ss(valueString);
  while (std::getline(ss, elem, separator)) {

    // If the end of the given string is an escape character, that
    // means this delimiter was escaped. Go get another piece of the
    // line and append it to this string before pushing it onto the
    // array.
    int lastchar = elem.size() - 1;
    if (elem[lastchar] == '\\') {

      elem[lastchar] = separator;
      out += elem;
      
    } else {

      out += elem;
      if (out.size() == 0) {
        vVal.push_back(elem);
      } else { 
        vVal.push_back(out);
        out = "";
      }
    }
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

  case VRCORETYPE_FLOAT:
    out = addData(name, (VRFloat)deserializeFloat(valueString));
    break;

  case VRCORETYPE_STRING:
    out = addData(name, (VRString)deserializeString(valueString));
    break;

  case VRCORETYPE_INTARRAY:
    out = addData(name, (VRIntArray)deserializeIntArray(valueString, separator));
    break;

  case VRCORETYPE_FLOATARRAY:
    out = addData(name, (VRFloatArray)deserializeFloatArray(valueString, separator));
    break;

  case VRCORETYPE_STRINGARRAY:
    out = addData(name, (VRStringArray)deserializeStringArray(valueString, separator));
    break;

  case VRCORETYPE_CONTAINER: {
    VRContainer emptyList;
    out = addData(name, emptyList);
    break;
  }

  case VRCORETYPE_NONE:
    out = "";
    break;

  }
  return out;
}

std::string VRDataIndex::walkXML(element* node, std::string nameSpace) {
  // This method will return the name of the last top-level element in
  // the input XML.

  std::string qualifiedName;
  std::string out;

  qualifiedName = nameSpace + std::string(node->get_name());

  // What type is this node?
  VRCORETYPE_ID typeId;
  if (node->get_value() == NULL) {
    // If the node has no value, we hope it's a container.
    typeId = VRCORETYPE_CONTAINER;
      
  } else if (node->get_attribute("type") == NULL) {

    if (node->has_child()) {

      typeId = VRCORETYPE_CONTAINER;
    } else {
    
      typeId = inferType(std::string(node->get_value()));
    }

  } else { // what does map return if no match?

    typeId = mVRTypeMap[std::string(node->get_attribute("type")->get_value())];

  }

  // Get a value for the node.
  std::string valueString;
  if (node->get_value() == NULL) {

    valueString = "";

  } else {

    // Convert to a string for some minor editing.
    valueString = std::string(node->get_value());

    // Trim leading and trailing spaces, tabs, whatever.
    int firstChar = valueString.find_first_not_of(" \t\r\n");
    int lastChar = valueString.find_last_not_of(" \t\r\n");

    if ((firstChar >= 0) && (lastChar >= 0)) {

      valueString = valueString.substr(firstChar, (1 + lastChar - firstChar));

    } else {

      valueString = "";
      typeId = VRCORETYPE_CONTAINER;
    }
  }

  char separator;
  if (node->get_attribute("separator") == NULL) {

    separator = MINVRSEPARATOR;

  } else {

    separator = *(node->get_attribute("separator")->get_value());
  }

  out = processValue(qualifiedName,
                     typeId,
                     valueString.c_str(),
                     separator);

  // There should be a datum object entered for this by here.  So now
  // we can see if there are any attributes to add to the list.
  VRDatum::VRAttributeList al = node->get_attribute_map();
  if (al.find("type") != al.end()) al.erase(al.find("type"));
  if (al.size() > 0) {
      
    getDatum(out)->setAttributeList(al);
  }

  // This loops through the node children, if there are any.
  VRContainer childNames;
  while (true) {

    // Pick the next child.
    element* child = node->get_next_child();
    if (child == NULL) {

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

  float conFloat = strtod(valueString.c_str(), &p);
  if (!*p) return VRCORETYPE_FLOAT;

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

    conFloat = strtod(valueString.substr(0, firstChar).c_str(), &p);
    if (!*p) return VRCORETYPE_FLOATARRAY;
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
  VRContainer outList;
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); it++) {
    outList.push_back(it->first);
  }
  return outList;
}

VRContainer VRDataIndex::selectByAttribute(const std::string attrName,
                                           const std::string attrVal) {
  std::list<std::string> outList;
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); it++) {
    VRDatum::VRAttributeList al = it->second->getAttributeList();

    // Check if attribute list has anything in it.
    if (!al.empty()) {

      // Yes? Loop through the attributes.
      for (VRDatum::VRAttributeList::iterator jt = al.begin();
           jt != al.end(); jt++) {

        // Do we have the correct attribute?
        if (attrName.compare(jt->first) == 0) {

          // Does it match the desired value, or a wildcard?
          if ((attrVal == "*") || (attrVal.compare(jt->second) == 0)) {

            // Put the name of the datum on the list.
            outList.push_back(it->first);
          }
        }
      }
    }
  }
  return outList;
}

VRContainer VRDataIndex::selectByAttribute(const std::string attrName,
	const std::string attrVal,
	const std::string nameSpace)
{
	std::string validatedNameSpace = validateNameSpace(nameSpace);
	std::list<std::string> outList;
	for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); it++) {
		VRDatum::VRAttributeList al = it->second->getAttributeList();
		if (validatedNameSpace.compare(0, getNameSpace(it->first).size(), getNameSpace(it->first)) == 0) {
			// Check if attribute list has anything in it.
			if (!al.empty()) {

				// Yes? Loop through the attributes.
				for (VRDatum::VRAttributeList::iterator jt = al.begin();
					jt != al.end(); jt++) {

					// Do we have the correct attribute?
					if (attrName.compare(jt->first) == 0) {

						// Does it match the desired value, or a wildcard?
						if ((attrVal == "*") || (attrVal.compare(jt->second) == 0)) {

							// Put the name of the datum on the list.
							outList.push_back(it->first);
						}
					}
				}
			}
		}
	}
	return outList;
}

VRContainer VRDataIndex::selectByType(const VRCORETYPE_ID typeId) {
  
  VRContainer outList;
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); it++) {

    if (typeId == it->second->getType()) {

      outList.push_back(it->first);
    }
  }
  return outList;
}

VRContainer VRDataIndex::selectByName(const std::string inName) {

  std::vector<std::string> inNameParts = explodeName(inName);
  VRContainer outList;

  // Sort through the whole index.
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); it++) {

    // This is our indicator.  If a name gets through all the
    // comparisons with test still equal to zero, it's a match.
    int test = 0;
    std::vector<std::string> nameParts = explodeName(it->first);

    if (inName[0] == '/') {

      // This is a full path name, so just start matching the two
      // names from their beginnings.
      int i;
      for (i = 0; i < inNameParts.size(); i++) {

        if (i >= nameParts.size()) break;
        if (!((inNameParts[i] == "*") ||
              (inNameParts[i].compare(nameParts[i]) == 0))) {
          test++;
          break;
        }
      }
      if (i < inNameParts.size()) test++;
        
    } else {

      // This part of the operation has two steps: find where to start the
      // the match testing, and then do the testing.
      //   Step 1: find the first match between the two exploded names.
      int i = 0;
      int j = 0;
      test = 1;

      for (i = 0; i < nameParts.size(); i++) {
        if ((inNameParts[j] == "*") ||
            (inNameParts[j].compare(nameParts[i]) == 0)) {
          test = 0; // There is a match. Signal it and get out.
          break;
        }
      } 
      //   Step 2: Does it match the rest of the string?
      if (test == 0) {
        while (++j < inNameParts.size()) {
          if (++i >= nameParts.size()) break;
          if (!((inNameParts[j] == "*") ||
                (inNameParts[j].compare(nameParts[i]) == 0))) {
            test = 1;
            break;
          }
        }
        // If we haven't exactly matched the inNameParts, flunk.
        if (j < inNameParts.size()) test++;
      }
    }

    if (test == 0) {
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

  if(out.size() == 0) out = "/";
  if (out[ 0 ] != '/') out = "/" + nameSpace;
  if (out[ out.size() - 1 ] != '/') out += '/';

  // If out is only one character, it's a '/' and we're done.
  if (out.size() > 1) {

    // Otherwise look for it in the index and throw an error if
    // it isn't there.
    if (mindex.find(out.substr(0, out.size() - 1)) == mindex.end()) {
      throw std::runtime_error("Can't find a namespace called " + nameSpace);
    }
  }

  return out;
}

// Returns the container name, derived from a long, fully-qualified, name.
std::string VRDataIndex::getNameSpace(const std::string fullName) {

  if (fullName.length() > 0) {

    // Start at the end.
    std::string::const_iterator it = fullName.end();

    int charLimit = 0;
    // March backward to the first '/' (if there was one).
    while (((*(--it)) != '/') && (charLimit < fullName.length())) { charLimit++; };

    // There was no slash at all.
    if (charLimit >= fullName.length()) return std::string("");
    
    // An obscure feature of the std::string constructor.
    return std::string(fullName.begin(), it + 1);

  } else {

    return std::string("");
  }

}

void VRDataIndex::pushState() {

  for (std::map<std::string, VRDatumPtr>::iterator it = mindex.begin();
       it != mindex.end(); it++) {
    it->second->push();
  }
}
  
void VRDataIndex::popState() {

  std::map<std::string, VRDatumPtr>::iterator it = mindex.begin();
  while (it != mindex.end()) {

    if (it->second->pop()) {
      mindex.erase(it++);

    } else {
      
      ++it;
    }
  }
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

  VRDataMap::iterator outIt;

  // If the input valName begins with a "/", it is a fully qualified
  // name already.  That is, it already includes the name space.
  if (valName[0] == '/') {

    return mindex.find(valName);

  } else {
    // If you're asking whether a fully-qualified name works, you want
    // a yes or no.  If you're asking about a relative name, you will
    // accept a name defined in a namespace senior to the one
    // specified.  So answering the query for an entry to match the
    // given name requires looking through the senior namespaces.
    
    std::string validatedNameSpace = validateNameSpace(nameSpace);
    
    // Separate the name space into its constituent elements.
    std::vector<std::string> elems = explodeName(validatedNameSpace);

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

  if (valName[0] != '/')
    throw std::runtime_error("Without a namespace, I can't find " + valName + ".");
  
  return getEntry(valName, "");
}

std::string VRDataIndex::getName(const std::string valName,
                                 const std::string nameSpace) {

  VRDataMap::iterator p = getEntry(valName, nameSpace);

  if (p == mindex.end()) {
    throw std::runtime_error("Never heard of " + valName + " in namespace " + nameSpace);
  } else {
    return p->first;
  }
}

// Returns the data object for this name.
VRDatumPtr VRDataIndex::getDatum(const std::string valName) {
  // If there is no namespace specified, assume the root space.  Note that this is
  // seldom what is really wanted, so it's better to try to use namespaces well.
  std::string testName;
  if (valName[0] != '/')
    testName = "/" + valName;
  else
    testName = valName;
  
  VRDataMap::const_iterator p = mindex.find(testName);

  if (p == mindex.end()) {
    throw std::runtime_error("Never heard of " + valName);
  } else {
    return p->second;
  }
}

VRDatumPtr VRDataIndex::getDatum(const std::string valName,
                                 const std::string nameSpace) {

  VRDataMap::iterator p = getEntry(valName, nameSpace);

  if (p == mindex.end()) {
    throw std::runtime_error("Never heard of " + valName + " in namespace " + nameSpace);
  } else {
    return p->second;
  }
}

std::string VRDataIndex::getTrimName(const std::string valName,
                                     const std::string nameSpace) {

  return getTrimName(validateNameSpace(nameSpace) + valName);
}

std::string VRDataIndex::getTrimName(const std::string valName) {

  std::string trimName;
  if (valName.find('/') == std::string::npos) {
    trimName = valName;

  } else {
 
    // This separates the valName on the slashes and puts the last
    // part of it into trimName.
    std::stringstream ss(valName);
    while (std::getline(ss, trimName, '/')) {};
  }
    
  return trimName;
}

// Returns the string representation of a name/value pair in the data index.
// Notice that you can't serialize the whole index.  That is, you need
// to specify an object or container to serialize, and "/" is not such
// a thing. To see why not, imagine what the serialization would look
// like: what would be inside the first XML tag?
std::string VRDataIndex::serialize(const std::string valName) {

  if (valName == "/")
    throw std::runtime_error("Can't serialize the whole index -- pick an object in it.");
  
  VRDataMap::iterator it = getEntry(valName, "");

  if (it != mindex.end()) {

    return serialize(it->first, it->second);

  } else {

    throw std::runtime_error(std::string("Never heard of ") + valName);

  }
}

std::string VRDataIndex::serialize(const std::string valName,
                                   const std::string nameSpace) {

  VRDataMap::iterator it = getEntry(valName, nameSpace);

  if (it != mindex.end()) {

    return serialize(it->first, it->second);

  } else {

    throw std::runtime_error("Never heard of " + valName + " in the namespace: " + nameSpace);

  }
}

// an int should be <nWindows type="int">6</nWindows>
std::string VRDataIndex::addSerializedValue(const std::string serializedData) {

  return addSerializedValue(serializedData, std::string(""));
}

std::string VRDataIndex::addSerializedValue(const std::string serializedData,
                                            const std::string nameSpace) {
  return addSerializedValue(serializedData, nameSpace, true);
}

std::string VRDataIndex::addSerializedValue(const std::string serializedData,
                                            const std::string nameSpace,
                                            const bool expand) {

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

  // If there are nodes in the tree with a 'linknode' attribute,
  // resolve them.
  if (expand) {
    linkNodes();
    linkContent();
  }
  
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

#ifdef DEBUG
    std::cout << "pathName: " << pathName << std::endl;
#endif
  } // End environment variable translation.

  return pathName;
}

bool VRDataIndex::processXML(const std::string arg) {

  if (arg.compare("-") == 0) {
    // If there's no filename, we expect piped input.
      
    std::string lineInput;
    std::stringstream buffer;

    while (std::cin >> lineInput) {
      buffer << lineInput + " ";
    }

    addSerializedValue(buffer.rdbuf()->str(), rootNameSpace);
                       
    return true;
      
  } else {
    // Read XML from the file.
    return processXMLFile( arg );
  }
}
  

// The default just loads the file values into the root namespace.
bool VRDataIndex::processXMLFile(const std::string fileName) {
  return processXMLFile(fileName, rootNameSpace);
}
  
bool VRDataIndex::processXMLFile(const std::string fileName,
                                 const std::string nameSpace) {

  std::string xml_string = "";
  std::string pathName = dereferenceEnvVars(fileName);

#ifdef DEBUG
  std::cout << "Reading from file = " << pathName << std::endl;
#endif
  ifstream file(pathName.c_str());

  if(file.is_open()) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    xml_string = buffer.rdbuf()->str();

    std::string ret = addSerializedValue(xml_string, nameSpace);
    
  } else {

    throw std::runtime_error("Error opening file " + fileName);

  }
  return true;
}

std::string VRDataIndex::addData(const std::string valName, VRInt value) {

  return addDataSpecialized<VRInt, VRCORETYPE_INT>(valName, value);
}

std::string VRDataIndex::addData(const std::string valName, VRFloat value) {

  return addDataSpecialized<VRFloat, VRCORETYPE_FLOAT>(valName, value);
}

std::string VRDataIndex::addData(const std::string valName, VRString value) {

  return addDataSpecialized<VRString, VRCORETYPE_STRING>(valName, value);
}

std::string VRDataIndex::addData(const std::string valName, VRIntArray value) {

  return addDataSpecialized<VRIntArray, VRCORETYPE_INTARRAY>(valName, value);
}

std::string VRDataIndex::addData(const std::string valName, VRFloatArray value) {

  return addDataSpecialized<VRFloatArray, VRCORETYPE_FLOATARRAY>(valName, value);
}

std::string VRDataIndex::addData(const std::string valName, VRStringArray value) {

  return addDataSpecialized<VRStringArray, VRCORETYPE_STRINGARRAY>(valName, value);
}

std::string VRDataIndex::addData(const std::string valName,
                                 VRContainer value) {

  // If the container to add to is the root, ignore.
  if (valName.compare("/") == 0)
    throw std::runtime_error(std::string("cannot replace the root namespace"));

  // All names must be in some namespace. If there is no namespace, put this
  // into the root namespace.
  std::string fixedValName = valName;
  if (valName[0] != '/') fixedValName = std::string("/") + valName;
  
  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(fixedValName);
  if (it == mindex.end()) {

    // No.  Create a new object.
    VRDatumPtr obj = factory.CreateVRDatum(VRCORETYPE_CONTAINER, &value);
    //std::cout << "added " << obj.containerVal()->getDatum() << std::endl;
    mindex.insert(VRDataMap::value_type(fixedValName, obj));

    // Add this value to the parent container, if any.
    VRContainer cValue;
    cValue.push_back(getTrimName(fixedValName));
    std::string ns = getNameSpace(fixedValName);
    // The parent container is the namespace minus the trailing /.
    if (ns.compare("/") != 0) addData(ns.substr(0,ns.size() - 1), cValue);
    
  } else {
    // Add value to existing container.
    it->second.containerVal()->addToValue(value);
  }
  return fixedValName;
}

// Prints the whole index.
std::string VRDataIndex::printStructure() {
  return printStructure("/");
}

std::string VRDataIndex::printStructure(const std::string itemName) {
  return printStructure(itemName, 50);
}

// Returns a printable description of the data structure that isn't
// XML.  You can always use 'serialize' instead, and see the XML, but
// I find this format to be easier to understand.  Also, the
// serialize() method does not use white space formatting and
// newlines.
std::string VRDataIndex::printStructure(const std::string itemName, const int lim) {
  
  int i;
  std::string outBuffer;

  // Get the pieces of the input name.
  std::vector<std::string> itemElems = explodeName( itemName );
  
  // We loop through *all* the values in the mindex, and only print
  // the ones that are asked for.
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); ++it) {

    bool printMe = true;

    // Get the pieces of the current name.
    std::vector<std::string> elems = explodeName( it->first );
    if (elems.size() == 0) continue;

    // Is it the same as the itemName?
    for (i = 1; i < min(itemElems.size(), elems.size()); i++) {
      
      if (itemElems[i].compare(elems[i]) != 0)
        printMe = false;
    }

    // No? Skip.
    if (!printMe) continue;

    // Yes, print it.  First calculate and print the indent.
    for (i = 0; i < ((int)elems.size() - 1); i++) outBuffer += " | ";

    // The name is being printed in context, so we only need the last
    // element of the exploded name.
    outBuffer += elems.back();

    if (it->second->getType() != VRCORETYPE_CONTAINER) {

      // This is not a container.  Get the value.
      std::string out = it->second->getValueString();

      // If there is a line limit, try to respect it in some vague fashion.
      if (out.size() > lim) {
        out = out.substr(0, (lim - 1)) + "...";
      }

      // Append the type description.
      outBuffer += " = " + out + " (" +  it->second->getDescription() + ")";
    }
      
    // Are there attributes?  If so, append them.
    VRDatum::VRAttributeList vrl = it->second->getAttributeList();
      
    if (vrl.size() > 0) {
      outBuffer += "\n";
      // Matches the indent above.
      for (i = 0; i < ((int)elems.size() - 1); i++) outBuffer += " | ";      
      outBuffer += "   [" + it->second->getAttributeListAsString() + " ]";
    }

    outBuffer += "\n";

  }

  return outBuffer;
}

// A recursive function that will copy a node as well as its children.
bool VRDataIndex::linkNode(const std::string fullSourceName,
                           const std::string fullTargetName,
                           int depthLimit) {

  // This is an easy way to make a disaster, so we have a recursion
  // limit.  It's possible this should be adjustable, but not sure of
  // a use case where the plausible depthLimit would exceed 10.
  if (depthLimit > 10)
    throw std::runtime_error("Too deep a recursion -- did you set up a circular reference?");

  // Find source node, fail if it does not exist.
  VRDataMap::iterator sourceEntry = getEntry(fullSourceName);
  if (sourceEntry == mindex.end())
    throw std::runtime_error("Can't find the source node: " + fullSourceName);
  
  VRDatumPtr sourceNode = sourceEntry->second;
  VRDataMap::iterator targetEntry = getEntry(fullTargetName);

  // Does this name already exist?
  if (targetEntry != mindex.end()) {

    // Yes.  Make the copy.
    targetEntry->second = sourceNode;
  } else {

    // No. Make an entry in the index, linked to the sourceNode.
    mindex.insert(VRDataMap::value_type(fullTargetName, sourceNode));
    
  }

  if (sourceNode->hasAttribute("linkContent"))
    throw std::runtime_error("You really don't want to mix linkContent and linkNode, as in linking " + fullSourceName + " and " + fullTargetName + ".");
  
  // If this is a container, recurse into the children, and copy them, too.
  if (sourceNode->getType() == VRCORETYPE_CONTAINER) {

    VRContainer childrenToCopy = sourceNode->getValue();

    // If there are no children, go home.
    if (childrenToCopy.empty()) return true;

    // Otherwise copy them, too.
    for (VRContainer::iterator jt = childrenToCopy.begin();
         jt != childrenToCopy.end(); jt++) 
      linkNode(fullSourceName + "/" + *jt,
               fullTargetName + "/" + *jt,
               depthLimit + 1);
  }
  return true;
}
    

// Implements a global link node operation.  That is, it looks for
// nodes with a 'linknode' attribute, and replaces them with a link to
// the node of the given name.
bool VRDataIndex::linkNodes() {

  // Find all the nodes that need copying.
  VRContainer linknodes = selectByAttribute("linkNode", "*");

  // Sift through them.
  for (VRContainer::iterator it = linknodes.begin();
       it != linknodes.end(); it++) {

    // Get the source name.
    std::string nameToCopy =
      getEntry(*it)->second->getAttributeValue("linkNode");

    // Check to see if we have a fully specified node name to copy.
    if (nameToCopy[0] == '/') {

      // Yes: just go ahead and copy it.
      linkNode(nameToCopy, *it);
      
    } else {
      // No: We have only a partial name, so need to find the
      // namespace within which to interpret it.  We will get that
      // namespace from the full name of the linknode element.
      std::string nameSpace = getNameSpace(*it);

      // Use getEntry with the name and namespace to select the node
      // to be copied.
      linkNode(getEntry(nameToCopy, nameSpace)->first, *it);
    }

    // Then just modify the entry in mindex so that mindex[*]->second
    // points to the found node's datum object.
  }
  return true;
}

// Implements a global link-to-contents operation.  Looks for
// container nodes with a 'linktocontents' attribute, and creates
// links within that namespace to the objects in the given namespace.
bool VRDataIndex::linkContent() {
  
  // Find all the containers to be replaced.
  VRContainer targets = selectByAttribute("linkContent", "*");

  // Sift through them.
  for (VRContainer::iterator it = targets.begin(); it != targets.end(); it++) {

    VRDataMap::iterator target = getEntry(*it);
    std::string targetNameSpace = getNameSpace(target->first);

    // Identify the source name (for the namespace) and the node.
    std::string sourceName =
      getEntry(target->second->getAttributeValue("linkContent"),
               targetNameSpace)->first;
    VRDatumPtr sourceNode = getEntry(sourceName)->second;
    if (sourceNode->getType() != VRCORETYPE_CONTAINER)
      throw std::runtime_error("Can only link from contents of containers, and " + sourceName + " is not a container.");
    
    // Read the contents of the sourceNode and create a new link for
    // each one, in the target name space.
    VRContainer relativeNameList = sourceNode->getValue();
    for (VRContainer::iterator jt = relativeNameList.begin();
         jt != relativeNameList.end(); jt++) {

      linkNode(validateNameSpace(sourceName) + *jt,
               targetNameSpace + *jt);
    }
    // The target node was only for holding the linkToContent
    // attribute, so is not needed any more.

    // Edit the parent container name list.
    if (targetNameSpace.size() > 1) {

      std::string targetParentName =
        targetNameSpace.substr(0, targetNameSpace.size() - 1);

      VRContainer oldNameList = getValue(targetParentName);
      
      // Construct a new list of names in the parent.
      std::string targetName = explodeName(target->first).back();
      // Start with the new names.
      VRContainer newList = relativeNameList;

      // Run through the old ones, and leave out the target name.
      for (VRContainer::iterator kt = oldNameList.begin();
           kt != oldNameList.end(); kt++) {
        if (targetName.compare(*kt) != 0) newList.push_back(*kt);
      }

      // Replace the parent name list.
      getDatum(targetParentName).containerVal()->setValue(newList);
    }

    // Delete the entry from the index.
    mindex.erase(target->first);
    // Note that you might have done something pathological with
    // linkNode that would result in a corrupted structure after this
    // removal.  That is, there might be another name in the index
    // linked to this node.  Don't do that.
  }
  return true;
}

}  // end namespace MinVR
