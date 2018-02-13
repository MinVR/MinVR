#include "VRDataIndex.h"

namespace MinVR {

std::string VRDataIndex::rootNameSpace = "/";

// Step 7 of the specialization instructions (in VRDatum.h) is to
// add an entry here to register the new data type.
VRDatumFactory VRDataIndex::_initializeFactory() {

  VRDatumFactory newFactory;

  newFactory.RegisterVRDatum(VRCORETYPE_INT, CreateVRDatumInt);
  newFactory.RegisterVRDatum(VRCORETYPE_FLOAT, CreateVRDatumFloat);
  newFactory.RegisterVRDatum(VRCORETYPE_STRING, CreateVRDatumString);
  newFactory.RegisterVRDatum(VRCORETYPE_INTARRAY, CreateVRDatumIntArray);
  newFactory.RegisterVRDatum(VRCORETYPE_FLOATARRAY, CreateVRDatumFloatArray);
  newFactory.RegisterVRDatum(VRCORETYPE_STRINGARRAY, CreateVRDatumStringArray);
  newFactory.RegisterVRDatum(VRCORETYPE_CONTAINER, CreateVRDatumContainer);

  return newFactory;
}

VRDatumFactory VRDataIndex::_factory = VRDataIndex::_initializeFactory();

VRDataIndex::VRDataIndex(const std::string serializedData)  :
  _indexName("MVR"), _overwrite(1), _linkNeeded(false) {

  _lastDatum = _theIndex.end();

  // If this is just a name, we just need an empty data index with the
  // given name.
  if (serializedData[0] != '<') {
    _indexName = serializedData;
    return;
  }

  // If you're here, the input data looks like XML. So parse it.
  Cxml *xml = new Cxml();
  xml->parse_string((char*)serializedData.c_str());
  element *xml_node = xml->get_root_element();
  element* child = xml_node->get_next_child();
  element* grandChild = child->get_next_child();
  std::string out;

  // If we have been given data from a valid XML file (which has only
  // one root element), the 'child' points to the root node, which we
  // don't want to keep.  So we'll work our way through the
  // grandchildren in this loop.
  while (child != NULL) {

    // There should be only one child here, so this should only pick
    // the root name off the serial data.
    _indexName = child->get_name();

    // Now scan through the child's children for the rest of the data.
    while (grandChild != NULL) {

      out = _walkXML(grandChild, "/");

      grandChild = child->get_next_child();
    }

    child = xml_node->get_next_child();
  }

  delete xml;

  // If there are nodes in the tree with a 'linknode' attribute,
  // resolve them.
  if (_linkNeeded) {
    _linkNodes();
    _linkContent();
    _linkNeeded = false;
  }

}

/// The copy constructor makes a deep copy.
VRDataIndex::VRDataIndex(const VRDataIndex& orig) {

  _indexName = orig._indexName;
  _overwrite = orig._overwrite;

  // Here's the shallow copy of the index.
  _theIndex = orig._theIndex;

  // Now we have to go through and do a deep copy of the data in the index.
  for (VRDataMap::iterator it = _theIndex.begin(); it != _theIndex.end(); it++) {
    it->second = it->second.clone();
  }

  // This part is not copied, but it's only a convenience, not part of the data.
  _lastDatum = _theIndex.end();

  // Then recreate the links.
  for (std::map<std::string, std::string>::const_iterator it = orig._linkRegister.begin();
       it != orig._linkRegister.end(); it++) {

    linkNode(it->first, it->second);
  }
}

std::string VRDataIndex::_getTrimName(const std::string &key,
                                     const std::string nameSpace) {

  std::string trimName;
  if (key.find('/') == std::string::npos) {
    trimName = key;

  } else {

    // This separates the key on the slashes and puts the last
    // part of it into trimName.
    std::stringstream ss(key);
    while (std::getline(ss, trimName, '/')) {};
  }

  return trimName;
}


std::string VRDataIndex::serialize() const {

    std::string serialized;

    serialized = "<" + getName() + " type=\"container\"" ;

    // This makes a list of *all* the names in the data index.
    // Unfortunately, we only want the first-level names, and want
    // none of the children of those first-level names.  So we have to
    // weed the second- and third-level names out.
    VRContainer nameList = findAllNames();
    if (nameList.empty()) {

      serialized += "/>";

    } else {

      serialized += ">";

      // ... loop through the children (recursively) ...
      for (VRContainer::iterator lt = nameList.begin();
           lt != nameList.end(); lt++) {

        // Search for a slash, not including the leading one.
        size_t slashPos = lt->find_first_of("/", 1);

        // If we didn't find it, this is a root-level name.
        if (slashPos == std::string::npos) {

          // ... recurse, and get the serialization of the member data value.
          serialized += serialize(*lt);
        };
      }
      serialized += "</" + getName() + ">";
    }

    return serialized;
}

// Returns the string representation of a name/value pair in the data index.
// If the argument is '/' you are probably trying to serialize the whole index,
// in which case the index name will be used as the root name.
std::string VRDataIndex::serialize(const std::string key,
                                   const std::string nameSpace,
                                   const bool inherit) const {

  if (key == "/") {

    return serialize();

  } else {
    VRDataMap::const_iterator it =
      const_cast<VRDataIndex*>(this)->_getEntry(key, nameSpace, inherit);

    if (it != _theIndex.end()) {

      return _serialize(it->first, it->second);

    } else {

      VRERRORNOADV("Never heard of " + key + " in the namespace: " + nameSpace);

    }
  }
}



std::string VRDataIndex::_serialize(const std::string &name,
                                    const VRDatumPtr &pdata ) const {

  std::string trimName = _getTrimName(name);

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

VRInt VRDataIndex::_deserializeInt(const char* valueString) {
  int iVal;
  sscanf(valueString, "%d", &iVal);

  return iVal;
}

VRFloat VRDataIndex::_deserializeFloat(const char* valueString) {
  float fVal;
  sscanf(valueString, "%f", &fVal);

  return fVal;
}

VRString VRDataIndex::_deserializeString(const char* valueString) {
  return std::string(valueString);
}

VRIntArray VRDataIndex::_deserializeIntArray(const char* valueString,
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

VRFloatArray VRDataIndex::_deserializeFloatArray(const char* valueString,
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

VRStringArray VRDataIndex::_deserializeStringArray(const char* valueString,
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
    int lastchar = (int)elem.size() - 1;
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

std::string VRDataIndex::_processValue(const std::string &name,
                                       VRCORETYPE_ID &type,
                                       const char* valueString,
                                       const char separator) {

  std::string out;
  /// Step 9 of adding a data type is adding entries to this switch.
  /// And the corresponding _deserialize*() method.
  switch (type) {
  case VRCORETYPE_INT:
    out = addData(name, (VRInt)_deserializeInt(valueString));
    break;

  case VRCORETYPE_FLOAT:
    out = addData(name, (VRFloat)_deserializeFloat(valueString));
    break;

  case VRCORETYPE_STRING:
    out = addData(name, (VRString)_deserializeString(valueString));
    break;

  case VRCORETYPE_INTARRAY:
    out = addData(name, (VRIntArray)_deserializeIntArray(valueString, separator));
    break;

  case VRCORETYPE_FLOATARRAY:
    out = addData(name, (VRFloatArray)_deserializeFloatArray(valueString, separator));
    break;

  case VRCORETYPE_STRINGARRAY:
    out = addData(name, (VRStringArray)_deserializeStringArray(valueString, separator));
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

std::string VRDataIndex::_walkXML(element* node, std::string nameSpace) {
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

      typeId = _inferType(std::string(node->get_value()));
    }

  } else {

    // Check to see if the type is one we can handle.
    VRDatum::VRTypeMap::iterator it =
      VRDatum::typeMap.find(std::string(node->get_attribute("type")->get_value()));

    if (it == VRDatum::typeMap.end()) {
      // If not, throw an error.
      VRERRORNOADV("No known type called " +
                   std::string(node->get_attribute("type")->get_value()));
    } else {
      // Otherwise, pass it along as the typeID.
      typeId = it->second;
    }
  }

  // Check to see if this is a link to anywhere.  If it is, signal
  // that we'll need the linking step after the deserialize.
  if (node->get_attribute("linkNode") != NULL) _linkNeeded = true;
  if (node->get_attribute("linkContent") != NULL) _linkNeeded = true;

  // Get a value for the node.
  std::string valueString;
  if (node->get_value() == NULL) {

    valueString = "";

  } else {

    // Convert to a string for some minor editing.
    valueString = std::string(node->get_value());

    // Trim leading and trailing spaces, tabs, whatever.
    int firstChar = (int)valueString.find_first_not_of(" \t\r\n");
    int lastChar = (int)valueString.find_last_not_of(" \t\r\n");

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

  out = _processValue(qualifiedName,
                     typeId,
                     valueString.c_str(),
                     separator);

  // There should be a datum object entered for this by here.  So now
  // we can see if there are any attributes to add to the list.
  VRDatum::VRAttributeList al = node->get_attribute_map();
  if (al.find("type") != al.end()) al.erase(al.find("type"));
  if (al.size() > 0) {

    _getDatum(out)->setAttributeList(al);
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
    _walkXML(child, qualifiedName + "/");
  }
}

  bool VRDataIndex::hasAttribute(const std::string &fullKey,
                                 const std::string &attributeName) const {
    return (_getDatum(fullKey)->getAttributeValue(attributeName).size() > 0);
  }

  std::string VRDataIndex::getAttributeValue(const std::string &fullKey,
                                             const std::string &attributeName) const {
    return _getDatum(fullKey)->getAttributeValue(attributeName);
  }

  void VRDataIndex::setAttributeValue(const std::string &fullKey,
                                      const std::string &attributeName,
                                      const std::string &attributeValue) {
    _getDatum(fullKey)->setAttributeValue(attributeName, attributeValue);
  }

// This function examines a value string and tries to determine what
// type it encodes.  It is used when the 'type=' attribute is missing.
// This is really just part of trying to make the package easy to use
// for configuration files.  For the serialize/deserialize pair, it's
// not an issue.
VRCORETYPE_ID VRDataIndex::_inferType(const std::string &valueString) const {
/// Step 10 -- Add some functionality to this method to help identify
/// your new data type.

  // Test for int
  char *p;
  int conInt = strtol(valueString.c_str(), &p, 10);
  if (!*p) return VRCORETYPE_INT;

  float conFloat = (float)strtod(valueString.c_str(), &p);
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

    conFloat = (float)strtod(valueString.substr(0, firstChar).c_str(), &p);
    if (!*p) return VRCORETYPE_FLOATARRAY;
  }

  // Not any of the above?  Probably a string.
  return VRCORETYPE_STRING;
}


// Just returns a list of the data names. For implementing an 'ls'
// command, or something like it.  If you want a list of names within
// a container (or within a namespace, pretty much the same thing),
// just use getValue().
std::list<std::string> VRDataIndex::findAllNames() const {
  VRContainer outList;
  for (VRDataMap::const_iterator it = _theIndex.begin(); it != _theIndex.end(); it++) {
    outList.push_back(it->first);
  }
  return outList;
}


VRContainer VRDataIndex::selectByAttribute(const std::string &attrName,
                                           const std::string &attrVal,
                                           const std::string nameSpace,
                                           const bool childOnly) const {

  std::string validatedNameSpace = validateNameSpace(nameSpace);

  std::list<std::string> outList;
	for (VRDataMap::const_iterator it = _theIndex.begin(); it != _theIndex.end(); it++) {
		VRDatum::VRAttributeList al = it->second->getAttributeList();

    // Use a string comparison to check if this name is within the given scope.
    int child = isChild(nameSpace, it->first);
    if ((childOnly && (child == 1)) || ((!childOnly) && (child >= 1))) {

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

std::string VRDataIndex::getByAttribute(const std::string &attrName,
                                        const std::string &attrVal,
                                        const std::string nameSpace) const {

  std::string out = "";
  std::string validatedNameSpace = validateNameSpace(nameSpace);
  size_t matchedLength = 0;

  // We are going to loop through all the names in the index to find the
  // longest string match to the input name space.  That *is* the match at the
  // lowest nested level.
	for (VRDataMap::const_iterator it = _theIndex.begin(); it != _theIndex.end(); it++) {

    // Use a string comparison to check if this name is within the given scope.
    std::string ns = _getNameSpace(it->first);
    if (ns.compare(0, ns.size(), validatedNameSpace, 0, ns.size()) == 0) {

      // If we're here, we have found a name that could contain the given
      // namespace.  If this one is not longer than the last, then skip it.
      if (ns.size() > matchedLength) {

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

                // Put the name of the datum into the proposed output and
                // record the length of the match.
                out = it->first;
                matchedLength = ns.size();
              }
            }
					}
				}
			}
		}
	}
	return out;
}

VRContainer VRDataIndex::selectByType(const VRCORETYPE_ID &typeID,
                                      const std::string nameSpace,
                                      const bool childOnly) const {

  VRContainer outList;
  for (VRDataMap::const_iterator it = _theIndex.begin(); it != _theIndex.end(); it++) {

    // Check to see if the type is the type we're looking for.
    if (typeID == it->second->getType()) {

      // Use a string comparison to check if this name is within the given scope.
      int child = isChild(nameSpace, it->first);
      if ((childOnly && (child == 1)) || ((!childOnly) && (child >= 1))) {

        // Put the name of the datum on the list.
        outList.push_back(it->first);
      }
    }
  }
  return outList;
}

VRContainer VRDataIndex::selectByKey(const std::string &inName,
                                     const std::string nameSpace) const {

  std::vector<std::string> inNameParts = _explodeName(nameSpace + inName);
  VRContainer outList;

  // Sort through the whole index.
  for (VRDataMap::const_iterator it = _theIndex.begin(); it != _theIndex.end(); it++) {

    // This is our indicator.  If a name gets through all the
    // comparisons with test still equal to zero, it's a match.
    int test = 0;
    std::vector<std::string> nameParts = _explodeName(it->first);

    if (inName[0] == '/') {

      // This is a full path name, so just start matching the two
      // names from their beginnings.
      size_t i;
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
      size_t i = 0;
      size_t j = 0;
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
std::vector<std::string> VRDataIndex::_explodeName(const std::string &fullName) {

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
//   3. Namespace ends with a "/" so keys can just be appended.
std::string VRDataIndex::validateNameSpace(const std::string &nameSpace) const {

  std::string out = nameSpace;

  if(out.size() == 0) out = "/";
  if (out[ 0 ] != '/') out = "/" + nameSpace;
  if (out[ out.size() - 1 ] != '/') out += '/';

  // If out is only one character, it's a '/' and we're done.
  if (out.size() > 1) {

    // Otherwise look for it in the index and throw an error if
    // it isn't there.
    if (_theIndex.find(out.substr(0, out.size() - 1)) == _theIndex.end()) {
      VRERRORNOADV("Can't find a namespace called " + nameSpace);
    }
  }

  return out;
}

int VRDataIndex::isChild(const std::string &parentName,
                         const std::string &childName) {

  // Remove any trailing slashes on the parent name.
  std::string pName = parentName;
  if (pName.size() > 0 && pName[pName.size() - 1] == '/') pName.erase(pName.size() - 1);

  // First check that the childName contains the pName, and is longer.
  int out = childName.compare(0, std::string::npos, pName) > 0;
  if (out > 0) {

    if (childName.compare(0, pName.size(), pName) != 0) return -1;

    // Count the slashes in what remains.
    std::string s = childName.substr(pName.size());
    return (int)std::count(s.begin(), s.end(), '/');

  } else if (out == 0) {

    // This seems redundant but is necessary to deal with compare() inconsistency.
    if (childName.size() != pName.size()) return -1;
    return 0;

  } else {

    return -1;
  }
}



// Returns the container name, derived from a long, fully-qualified, name.
std::string VRDataIndex::_getNameSpace(const std::string &fullName) {

  if (fullName.length() > 0) {

    // Start at the end.
    std::string::const_reverse_iterator it = fullName.rbegin();

    size_t charLimit = 0;
    // March backward to the first '/' (if there was one).
    while (((*(++it)) != '/') && (charLimit < fullName.length())) { charLimit++; };

    // There was no slash at all.
    if (charLimit >= fullName.length()) return std::string("");

    // An obscure feature of the std::string constructor.
    return std::string(fullName.begin(), it.base());

  } else {

    return std::string("");
  }

}

void VRDataIndex::pushState() {

  for (std::map<std::string, VRDatumPtr>::iterator it = _theIndex.begin();
       it != _theIndex.end(); it++) {
    it->second->push();
  }
}

// Values that were added to the index after a push will be deleted on a pop,
// but the system cannot restore deleted values.  Fortunately, there is no
// delete method for the index.  When one is added, this will get more
// complicated.
void VRDataIndex::popState() {

  std::map<std::string, VRDatumPtr>::iterator it = _theIndex.begin();
  while (it != _theIndex.end()) {

    if (it->second->pop()) {
      _theIndex.erase(it++);

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
VRDataIndex::_getEntry(const std::string &key,
                       const std::string nameSpace,
                       const bool inherit) {

  // If the input key begins with a "/", it is a fully qualified
  // name already.  That is, it already includes the name space.
  if (key[0] == '/') {

    _lastDatum = _theIndex.find(key);
    return _lastDatum;

  } else if ((nameSpace.size() == 1) && (nameSpace.compare("/") == 0)) {
    // If we are looking for something in the root name space, that's
    // not so different than using a fully-qualified name.

    _lastDatum = _theIndex.find(nameSpace + key);
    return _lastDatum;

  } else {

    // If you're asking whether a fully-qualified name works, you want
    // a yes or no answer.  If you're asking about a relative name, you will
    // accept a name defined in a namespace senior to the one
    // specified.  So answering the query for an entry to match the
    // given name requires looking through the senior namespaces.

    std::string validatedNameSpace = validateNameSpace(nameSpace);

    // If inheritance is turned off, just check if this name exists.
    if (!inherit) {
      _lastDatum = _theIndex.find(validatedNameSpace + key);
      return _lastDatum;
    }

    // Separate the name space into its constituent elements.
    std::vector<std::string> elems = _explodeName(validatedNameSpace);

    // We start from the longest name space and peel off the rightmost
    // element each iteration until we find a match, or not.  This
    // provides for the most local version of key to prevail.  The
    // last iteration creates an empty testSpace, on purpose, to test
    // the root level nameSpace.
    for (int N = (int)elems.size(); N >= 0; --N) {

      std::vector<std::string> names(&elems[0], &elems[0] + N);
      std::string testSpace;

      for (std::vector<std::string>::iterator it = names.begin();
           it != names.end(); ++it) {

        testSpace += *it + "/" ;
      }

      _lastDatum = _theIndex.find(testSpace + key);
      if (_lastDatum != _theIndex.end()) {
        return _lastDatum;
      }
    }

    // If we are here, there is no matching name in the index.
    _lastDatum = _theIndex.end();
    return _lastDatum;
  }
}

std::string VRDataIndex::getFullKey(const std::string &key,
                                    const std::string nameSpace,
                                    const bool inherit) const {

  VRDataMap::const_iterator p =
    const_cast<VRDataIndex*>(this)->_getEntry(key, nameSpace, inherit);

  if (p == _theIndex.end()) {
      VRERRORNOADV("Uh-oh. Never heard of " + key + " in namespace " + nameSpace);
  } else {
    return p->first;
  }
}

// Returns the data object for this name.
VRDatumPtr VRDataIndex::_getDatum(const std::string &key,
                                  const std::string nameSpace,
                                  const bool inherit) {

  VRDataMap::iterator p = _getEntry(key, nameSpace, inherit);

  if (p == _theIndex.end()) {
    VRERRORNOADV("What? Never heard of " + key + " in namespace " + nameSpace);
  } else {
    return p->second;
  }
}

// Returns the data object for this name.
const VRDatumPtr VRDataIndex::_getDatum(const std::string &key,
                                  const std::string nameSpace,
                                  const bool inherit) const {

    VRDataMap::const_iterator p =
      const_cast<VRDataIndex*>(this)->_getEntry(key, nameSpace, inherit);

    if (p == _theIndex.end()) {
        VRERRORNOADV("Who? Never heard of " + key + " in namespace " + nameSpace);
    } else {
        return p->second;
    }
}

// an int should be <nWindows type="int">6</nWindows>
std::string VRDataIndex::addSerializedValue(const std::string serializedData,
                                            const std::string nameSpace,
                                            const bool expand) {

  Cxml *xml = new Cxml();
  xml->parse_string((char*)serializedData.c_str());
  element *xml_node = xml->get_root_element();
  element* child = xml_node->get_next_child();
  std::string out;

  while (child != NULL) {

    out = _walkXML(child, validateNameSpace(nameSpace));

    child = xml_node->get_next_child();
  }

  delete xml;

  // If there are nodes in the tree with a 'linknode' attribute,
  // resolve them.
  if (expand && _linkNeeded) {
    _linkNodes();
    _linkContent();
    _linkNeeded = false;
  }

  return out;
}

std::string VRDataIndex::dereferenceEnvVars(const std::string &fileName) {

  // A little loop to accommodate environment variables in the
  // fileName specification. The variables are assumed to be packaged
  // inside a '${}' combination, e.g. ${MVRHOME}/tests/config/test.xml
  //
  std::string pathName = fileName;
  size_t dollarPos = 0;

  dollarPos = pathName.find_first_of("$", dollarPos);
  while (dollarPos != std::string::npos) {

    size_t bracketPos = pathName.find_first_of("}", dollarPos);

    if (bracketPos == string::npos) {
      VRERROR("Bad environment variable syntax.",
              "Environment variables must be enclosed like ${THIS}.");
    }

    size_t bracketLen = 1 + bracketPos - dollarPos;
    std::string envVariable = pathName.substr(dollarPos + 2, bracketLen - 3);

    if (getenv(envVariable.c_str()) == NULL) {
      VRERRORNOADV("No such environment variable: " + envVariable);
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

    VRERRORNOADV("Error opening file " + fileName + ".");

  }
  return true;
}

std::string VRDataIndex::addData(const std::string &key, VRInt value) {

  return addDataSpecialized<VRInt, VRCORETYPE_INT>(key, value);
}

std::string VRDataIndex::addData(const std::string &key, VRFloat value) {

  return addDataSpecialized<VRFloat, VRCORETYPE_FLOAT>(key, value);
}

std::string VRDataIndex::addData(const std::string &key, VRString value) {

  return addDataSpecialized<VRString, VRCORETYPE_STRING>(key, value);
}

std::string VRDataIndex::addData(const std::string &key, VRIntArray value) {

  return addDataSpecialized<VRIntArray, VRCORETYPE_INTARRAY>(key, value);
}

std::string VRDataIndex::addData(const std::string &key, VRFloatArray value) {

  return addDataSpecialized<VRFloatArray, VRCORETYPE_FLOATARRAY>(key, value);
}

std::string VRDataIndex::addData(const std::string &key, VRStringArray value) {

  return addDataSpecialized<VRStringArray, VRCORETYPE_STRINGARRAY>(key, value);
}

std::string VRDataIndex::addData(const std::string &key,
                                 VRContainer value) {

  // If the container to add to is the root, ignore.
  if (key.compare("/") == 0)
    VRERRORNOADV("Cannot replace the root namespace");

  // All names must be in some namespace. If there is no namespace, put this
  // into the root namespace.
  std::string fixedValName = key;
  if (key[0] != '/') fixedValName = std::string("/") + key;

  // Check if the name is already in use.
  VRDataMap::iterator it = _theIndex.find(fixedValName);
  if (it == _theIndex.end()) {

    // No.  Create a new object.
    VRDatumPtr obj = _factory.CreateVRDatum(VRCORETYPE_CONTAINER, &value);
    //std::cout << "added " << obj.containerVal()->_getDatum() << std::endl;
    _theIndex.insert(VRDataMap::value_type(fixedValName, obj));

    // Add this value to the parent container, if any.
    VRContainer cValue;
    cValue.push_back(_getTrimName(fixedValName));
    std::string ns = _getNameSpace(fixedValName);
    // The parent container is the namespace minus the trailing /.
    if (ns.compare("/") != 0) addData(ns.substr(0,ns.size() - 1), cValue);

  } else {
    // Add value to existing container.
    it->second.containerVal()->addToValue(value);
  }
  return fixedValName;
}

std::string VRDataIndex::addData(const std::string &keyAndValue) {

  size_t poseql = keyAndValue.find("=");
  if (poseql == std::string::npos) {
    VRERRORNOADV("MinVR Error: Expected a key=value format for the string: " +
                 keyAndValue);
  }
  std::string key = keyAndValue.substr(0,poseql);
  std::string value = keyAndValue.substr(poseql+1);

  //  std::cout << "MinVR: Setting config value: " << key << "=" << value << std::endl;

  // If the key already exists, then match the existing type, otherwise infer
  // the type as usual.
  VRCORETYPE_ID type;
  if (exists(key, "/")) {
    type = getType(key);

  } else {

    type = _inferType(value);
  }

  // Got the name, type, value.  Go ahead and insert it into the index.
  return _processValue(key, type, value.c_str(), MINVRSEPARATOR);
}

// Returns a printable description of the data structure that isn't
// XML.  You can always use 'serialize' instead, and see the XML, but
// I find this format to be easier to understand.  Also, the
// serialize() method does not use white space formatting and
// newlines.
std::string VRDataIndex::printStructure(const std::string itemName,
                                        const size_t lim) const {
  size_t i;
  std::string outBuffer;

  // Get the pieces of the input name.
  std::vector<std::string> itemElems = _explodeName( itemName );

  // If we're printing the entire index, prepend the index name.
  if (itemName.compare("/") == 0) outBuffer += _indexName + "\n";

  // We loop through *all* the values in the _theIndex, and only print
  // the ones that are asked for.
  for (VRDataMap::const_iterator it = _theIndex.begin(); it != _theIndex.end(); ++it) {

    bool printMe = true;

    // Get the pieces of the current name.
    std::vector<std::string> elems = _explodeName( it->first );
    if (elems.size() == 0) continue;

    // Is it the same as the itemName?
    for (i = 1; i < min(itemElems.size(), elems.size()); i++) {

      if (itemElems[i].compare(elems[i]) != 0)
        printMe = false;
    }

    // No? Skip.
    if (!printMe) continue;

    // Yes, print it.  First calculate and print the indent.
    for (i = 0; i < (elems.size() - 1); i++) outBuffer += " | ";

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
      for (i = 0; i < (elems.size() - 1); i++) outBuffer += " | ";
      outBuffer += "   [" + it->second->getAttributeListAsString() + " ]";
    }

    outBuffer += "\n";

  }

  return outBuffer;
}



// A recursive function that will copy a node as well as its children.
bool VRDataIndex::linkNode(const std::string &fullSourceName,
                           const std::string &fullTargetName,
                           int depthLimit) {

  // This is an easy way to make a disaster, so we have a recursion
  // limit.  It's possible this should be adjustable, but not sure of
  // a use case where the plausible depthLimit would exceed 10.
  if (depthLimit > 7)
    VRERROR("Too deep a recursion.", "Did you set up a circular reference?");

  // Find source node, fail if it does not exist.
  VRDataMap::iterator sourceEntry = _getEntry(fullSourceName);
  if (sourceEntry == _theIndex.end())
    VRERRORNOADV("Can't find the source node: " + fullSourceName);

  // It's possible the target name won't start with a '/', in which
  // case, stick one on front.
  std::string fixTargetName =
    (fullTargetName[0] == '/') ? fullTargetName : "/" + fullTargetName;

  
  VRDatumPtr sourceNode = sourceEntry->second;
  VRDataMap::iterator targetEntry = _getEntry(fixTargetName);

  // Does this name already exist?
  if (targetEntry != _theIndex.end()) {

    // Yes.  Make the copy.
    targetEntry->second = sourceNode;
  } else {

    // No. Make an entry in the index, linked to the sourceNode.
    _theIndex.insert(VRDataMap::value_type(fixTargetName, sourceNode));

  }

  if (sourceNode->hasAttribute("linkContent"))
    VRERROR("Linking content and nodes not allowed.",
            "You really don't want to mix linkContent and linkNode, as in linking " +
            fullSourceName + " and " + fixTargetName + ".");

  // Record the link we made.  This is for use by the copy constructor.
  _linkRegister[fullSourceName] = fixTargetName;

  // If this is a container, recurse into the children, and copy them, too.
  if (sourceNode->getType() == VRCORETYPE_CONTAINER) {

    VRContainer childrenToCopy = sourceNode->getValue();

    // If there are no children, go home.
    if (childrenToCopy.empty()) return true;

    // Otherwise copy them, too.
    for (VRContainer::iterator jt = childrenToCopy.begin();
         jt != childrenToCopy.end(); jt++)
      linkNode(fullSourceName + "/" + *jt,
               fixTargetName + "/" + *jt,
               depthLimit + 1);
  }
  return true;
}


// Implements a global link node operation.  That is, it looks for
// nodes with a 'linknode' attribute, and replaces them with a link to
// the node of the given name.
bool VRDataIndex::_linkNodes() {

  // Find all the nodes that need copying.
  VRContainer linknodes = selectByAttribute("linkNode", "*");

  // Sift through them.
  for (VRContainer::iterator it = linknodes.begin();
       it != linknodes.end(); it++) {

    // Get the source name.
    std::string nameToCopy =
      _getEntry(*it)->second->getAttributeValue("linkNode");

    // Check to see if we have a fully specified node name to copy.
    if (nameToCopy[0] == '/') {

      // Yes: just go ahead and copy it.
      linkNode(nameToCopy, *it);

    } else {
      // No: We have only a partial name, so need to find the
      // namespace within which to interpret it.  We will get that
      // namespace from the full name of the linknode element.
      std::string nameSpace = _getNameSpace(*it);

      // Use getEntry with the name and namespace to select the node
      // to be copied.
      linkNode(_getEntry(nameToCopy, nameSpace)->first, *it);
    }

    // Then just modify the entry in _theIndex so that _theIndex[*]->second
    // points to the found node's datum object.
  }
  return true;
}

// Implements a global link-to-contents operation.  Looks for
// container nodes with a 'linktocontents' attribute, and creates
// links within that namespace to the objects in the given namespace.
bool VRDataIndex::_linkContent() {

  // Find all the containers to be replaced.
  VRContainer targets = selectByAttribute("linkContent", "*");

  // Sift through them.
  for (VRContainer::iterator it = targets.begin(); it != targets.end(); it++) {

    VRDataMap::const_iterator target = _getEntry(*it);
    std::string targetNameSpace = _getNameSpace(target->first);

    // Identify the source name (for the namespace) and the node.
    VRDataMap::iterator linkEntry =
      _getEntry(target->second->getAttributeValue("linkContent"));
    if (linkEntry == _theIndex.end()) {
      VRERROR("Can't find link target: " +
              target->second->getAttributeValue("linkContent"),
              "Check the spelling?");
    }

    std::string sourceName =
      _getEntry(target->second->getAttributeValue("linkContent"),
               targetNameSpace)->first;
    VRDatumPtr sourceNode = _getEntry(sourceName)->second;
    if (sourceNode->getType() != VRCORETYPE_CONTAINER)
      VRERROR("Link disallowed.",
              "Can only link from contents of containers, and " +
              sourceName + " is not a container.");

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
      std::string targetName = _explodeName(target->first).back();
      // Start with the new names.
      VRContainer newList = relativeNameList;

      // Run through the old ones, and leave out the target name.
      for (VRContainer::iterator kt = oldNameList.begin();
           kt != oldNameList.end(); kt++) {
        if (targetName.compare(*kt) != 0) newList.push_back(*kt);
      }

      // Replace the parent name list.
      _getDatum(targetParentName).containerVal()->setValue(newList);
    }

    // Delete the entry from the index.
    _theIndex.erase(target->first);
    // Note that you might have done something pathological with
    // linkNode that would result in a corrupted structure after this
    // removal.  That is, there might be another name in the index
    // linked to this node.  Don't do that.
  }
  return true;
}

}  // end namespace MinVR

// This is stuck here in case it's useful for debugging an improved XML parser.
// Otherwise of no importance.
//
// bool VRDataIndex::printXML(element* node, std::string prefix) {

//   std::string newPrefix = prefix + std::string("| ");

//   std::cout << newPrefix << "Node: " << node->get_name() << std::endl;
//   if (node->get_value())
//     std::cout << newPrefix << "Value: >" << node->get_value() << "<" << std::endl;

//   attribute *attr = node->get_next_attribute();
//   while (attr != NULL) {
//     std::cout << newPrefix << "| | Attribute: " << attr->get_name() << std::endl;
//     if (attr->get_value())
//       std::cout << newPrefix << "| | Value: " << attr->get_value() << std::endl;

//     attr = node->get_next_attribute();
//   }

//   element *child = node->get_next_child();
//   while (child != NULL) {

//     printXML(child, newPrefix);

//     child = node->get_next_child();
//   }
//   node->reset_iterators();

//   return true;

// }

