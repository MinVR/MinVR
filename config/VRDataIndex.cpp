#include "VRDataIndex.h"

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


std::string VRDataIndex::getDescription(const std::string valName) {
  return ("<" + getTrimName(valName) + " type=\"" +
          getDatum(valName)->getDescription() + "\"/>");
}

std::string VRDataIndex::getDescription(const std::string valName,
                                        const std::string nameSpace) {

  return ("<" + getTrimName(valName, nameSpace) + " type=\"" +
          getDatum(valName, nameSpace)->getDescription() + "\"/>");
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

bool VRDataIndex::processXMLFile(std::string fileName, std::string nameSpace) {

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

  // Should sort mindex here.

  int i;
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); ++it) {

    std::vector<std::string> elems = explodeName( it->first );

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


