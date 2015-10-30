#include "VRDataIndex.h"

// Just returns a list of the data names. For implementing an 'ls'
// command, or something like it.
std::list<std::string> VRDataIndex::getDataNames() {
  std::list<std::string> outList;
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); it++) {
    outList.push_back(it->first);
  }
  return outList;
}

//
std::list<std::string> VRDataIndex::getDataNames(const std::string containerName) {
  std::list<std::string> outList;
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); it++) {

    // XXX THIS IS NOT RIGHT -- comparison should be for beginning of string.
    if (it->first.compare(containerName)) {
      outList.push_back(it->first);
    }
  }
  return outList;
}

// Breaks up a name into its constituent parts, on the slashes.
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

  if (out[0] != '/') {
    out = "/" + nameSpace;
  }

  if (mindex.find(out) == mindex.end()) {

    throw("Can't find a namespace called " + nameSpace);

  }

  return out + "/";
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

  return getTrimName(nameSpace + valName);
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
  return ("<" + getTrimName(valName) + " type=\"" + getDatum(valName)->getDescription() + "\"/>");
}

std::string VRDataIndex::getDescription(const std::string valName,
                                        const std::string nameSpace) {

  std::string ns = validateNameSpace(nameSpace);

  return ("<" + getTrimName(valName, ns) +
          " type=\"" + getDatum(valName, ns)->getDescription() +
          "\"/>");
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
bool VRDataIndex::addSerializedValue(const std::string serializedData) {

  return addSerializedValue(serializedData, std::string(""));
}

bool VRDataIndex::addSerializedValue(const std::string serializedData,
                                        const std::string nameSpace) {
  Cxml *xml = new Cxml();
  xml->parse_string((char*)serializedData.c_str());
  element *xml_node = xml->get_root_element();
  element* child = xml_node->get_next_child();

  while (child != NULL) {

#ifdef DEBUG
    printXML(child, nameSpace);
#endif
    walkXML(child, nameSpace);

    child = xml_node->get_next_child();
  }

  delete xml;
}

bool VRDataIndex::processXMLFile(std::string fileName) {

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
      printXML(child, std::string(""));
#endif
      walkXML(child, std::string(""));

      child = xml_node->get_next_child();
    }

    delete xml;

  } else {
    std::cout << "Error opening file " << fileName << std::endl;
  }
}

std::string VRDataIndex::addValue(const std::string valName, int value) {

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    // No? Create it and stick it in index.
    VRDatumPtr obj = factory.CreateVRDatum(MVRINT, &value);
    mindex.insert(VRDataMap::value_type(valName, obj));

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

std::string VRDataIndex::addValue(const std::string valName, double value) {

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    VRDatumPtr obj = factory.CreateVRDatum(MVRDOUBLE, &value);
    mindex.insert(VRDataMap::value_type(valName, obj));
    return valName;

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

std::string VRDataIndex::addValue(const std::string valName, std::string value) {

  // Remove leading spaces.
  int valueBegin = value.find_first_not_of(" \t\n\r");
  if (valueBegin == value.size())
    return false; // no content. This should not happen. Or should it?

  // ... and trailing.
  int valueEnd = value.find_last_not_of(" \t\n\r");
  int valueRange = valueEnd - valueBegin + 1;

  std::string trimValue = value.substr(valueBegin, valueRange);

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    VRDatumPtr obj = factory.CreateVRDatum(MVRSTRING, &trimValue);
    mindex.insert(VRDataMap::value_type(valName, obj));
    return valName;

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

std::string VRDataIndex::addValue(const std::string valName, MVRArrayDouble value) {

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    // No? Create it and stick it in index.
    VRDatumPtr obj = factory.CreateVRDatum(MVRARRAYDOUBLE, &value);
    mindex.insert(VRDataMap::value_type(valName, obj));
    return valName;

  } else {
    // Overwrite value
    if (overwrite > 0) {
      it->second.arrayDoubleVal()->setValue(value);
    } else if (overwrite == 0) {
      throw std::runtime_error(std::string("overwriting values not allowed"));
    }
  }
  return valName;
}


std::string VRDataIndex::addValue(const std::string valName,
                                  MVRContainer value) {

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    VRDatumPtr obj = factory.CreateVRDatum(MVRCONTAINER, &value);
    //std::cout << "added " << obj.containerVal()->getDatum() << std::endl;
    mindex.insert(VRDataMap::value_type(valName, obj));

  } else {
    // Add value to existing container.
    it->second.containerVal()->addToValue(value);
  }
  return valName;
}

std::string VRDataIndex::addValue(const std::string valName) {

  // Check if the name is already in use.
  VRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    MVRContainer v ;
    VRDatumPtr obj = factory.CreateVRDatum(MVRCONTAINER, &v);
    //std::cout << "added " << obj.containerVal()->getDatum() << std::endl;
    mindex.insert(VRDataMap::value_type(valName, obj));

  }
  return valName;
}



// Removes a value from the index. If this is a container, we must also
// remove the member values.
void VRDataIndex::rmValue(const std::string valName, const std::string nameSpace) {

  VRDataMap::iterator it = getEntry(valName, nameSpace);

  // If this is a container, remove the contents of the container.
  if (it->second->getType() == MVRCONTAINER) {

    MVRContainer value = it->second->getValue();

    for (MVRContainer::iterator vit = value.begin(); vit != value.end(); vit++) {
      rmValue(*vit, it->first + "/");
    }
  }

  // If this value is *in* a container, edit that container's list.
  std::vector<std::string> elems = explodeName(it->first);

  // Construct the name of the parent container.
  std::string parent;
  for (int i = 0; i < elems.size() - 1; i++) {
    parent += "/" + elems[i];
  }

  // If we're not at the root level, edit the appropriate container.
  if (! parent.compare("/")) {
    VRDataMap::iterator pit = getEntry(parent);

    pit->second.containerVal()->removeValue(elems[elems.size() - 1]);

  }
}

void VRDataIndex::rmValue(const std::string valName) {

  rmValue(valName, "");
}

void VRDataIndex::printWholeKitAndKaboodle() {

  // Should sort mindex here.

  int i;
  for (VRDataMap::iterator it = mindex.begin(); it != mindex.end(); ++it) {

    std::vector<std::string> elems = explodeName( it->first );

    for (i = 0; i < elems.size() - 1; i++) std::cout << " | ";
    std::cout << elems.back();

    if (it->second->getType() == MVRCONTAINER) {

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


