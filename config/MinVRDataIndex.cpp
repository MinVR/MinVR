#include "MinVRDatum.h"
#include "MinVRDatumFactory.h"
#include "MinVRDataIndex.h"

// Step 5 of the specialization instructions (in MinVRDatum.h) is to
// add an entry here to register the new data type.
MinVRDataIndex::MinVRDataIndex() : overwrite(true) {
  factory.RegisterMinVRDatum(MVRINT, CreateMinVRDatumInt);
  factory.RegisterMinVRDatum(MVRFLOAT, CreateMinVRDatumDouble);
  factory.RegisterMinVRDatum(MVRSTRING, CreateMinVRDatumString);
  factory.RegisterMinVRDatum(MVRCONTAINER, CreateMinVRDatumContainer);


  mvrTypeMap[std::string("int")] = MVRINT;
  mvrTypeMap[std::string("float")] = MVRFLOAT;
  mvrTypeMap[std::string("string")] =  MVRSTRING;
  mvrTypeMap[std::string("vector<int>")] =  MVRVEC_INT;
  mvrTypeMap[std::string("vector<float>")] =  MVRVEC_FLOAT;
  mvrTypeMap[std::string("vector<string>")] =  MVRVEC_STRING;
  mvrTypeMap[std::string("container")] =  MVRCONTAINER;

}

std::list<std::string> MinVRDataIndex::getDataNames() {
  std::list<std::string> outList;
  for (std::map<std::string, MinVRDatumPtr>::iterator it = mindex.begin();
       it != mindex.end(); it++) {
    outList.push_back(it->first);
  }
  return outList;
}

bool MinVRDataIndex::addValueInt(const std::string valName, int value) {

  // Check if the name is already in use.
  MinVRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    // No? Create it and stick it in index.
    MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRINT, &value);
    return mindex.insert(MinVRDataMap::value_type(valName, obj)).second;

  } else {
    // Overwrite value
    if (overwrite) {
      return it->second.intVal()->setValue(value);
    } else {
      return false;
    }
  }
}

bool MinVRDataIndex::addValueDouble(const std::string valName, double value) {

  // Check if the name is already in use.
  MinVRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRFLOAT, &value);
    //std::cout << "added " << obj.doubleVal()->getValue() << std::endl;
    return mindex.insert(MinVRDataMap::value_type(valName, obj)).second;

  } else {
    // Overwrite value
    if (overwrite) {
      return it->second.doubleVal()->setValue(value);
    } else {
      return false;
    }
  }
}

bool MinVRDataIndex::addValueString(const std::string valName, std::string value) {

  // Remove leading spaces.
  int valueBegin = value.find_first_not_of(" \t\n\r");
  if (valueBegin == value.size())
    return false; // no content. This should not happen. Or should it?

  // ... and trailing.
  int valueEnd = value.find_last_not_of(" \t\n\r");
  int valueRange = valueEnd - valueBegin + 1;

  std::string trimValue = value.substr(valueBegin, valueRange);

  // Check if the name is already in use.
  MinVRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRSTRING, &trimValue);
    //std::cout << "added " << obj.stringVal()->getValue() << std::endl;
    return mindex.insert(MinVRDataMap::value_type(valName, obj)).second;

  } else {
    // Overwrite value
    if (overwrite) {
      return it->second.stringVal()->setValue(trimValue);
    } else {
      return false;
    }
  }
}

bool MinVRDataIndex::addValueContainer(const std::string valName,
                                       std::list<std::string> value) {

  // Check if the name is already in use.
  MinVRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {

    MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRCONTAINER, &value);
    //std::cout << "added " << obj.containerVal()->getValue() << std::endl;
    return mindex.insert(MinVRDataMap::value_type(valName, obj)).second;
  } else {
    // Add value to existing container.
    return it->second.containerVal()->addToValue(value);
  }
}

MinVRDatumPtr MinVRDataIndex::getValue(const std::string valName) {
  MinVRDataMap::const_iterator it = mindex.find(valName);
  if (it == mindex.end()) {
    throw std::runtime_error(std::string("never heard of ") + valName);
  } else {
    return it->second;
  }
}

// This is the same as getValue(), except that it allows the caller to
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
MinVRDatumPtr MinVRDataIndex::getValue(const std::string valName,
                                       const std::string nameSpace) {
  std::vector<std::string> elems;
  std::string elem;
  std::stringstream ss(nameSpace);
  while (std::getline(ss, elem, '/')) {
    elems.push_back(elem);
    std::cout << "found: " << elem << std::endl;
  }

  // We start from the longest name space and peel off the rightmost
  // element each iteration until we find a match, or not.  This
  // provides for the most local version of valName to prevail.  The
  // last iteration creates an empty testSpace, on purpose.
  for (int N = elems.size(); N > 0; --N) {

    std::vector<std::string> names(&elems[0], &elems[N]);
    std::string testSpace;

    for (std::vector<std::string>::iterator it = names.begin();
         it != names.end(); ++it) {

      testSpace += *it + "/" ;
    }

    std::cout << "testing: " << testSpace + valName << std::endl;
    MinVRDataMap::const_iterator it = mindex.find(testSpace + valName);
    if (it != mindex.end()) {
      return it->second;
    }
  }

  throw std::runtime_error(std::string("never heard of ") + valName + std::string(" in any of the namespaces: ") + nameSpace);

}

std::string MinVRDataIndex::getDescription(const std::string valName) {
  return ("<" + valName + " type=\"" + getValue(valName)->getDescription() + "\"/>");
}

std::string MinVRDataIndex::getDescription(const std::string valName,
                                           const std::string nameSpace) {
  return ("<" + valName +
          " type=\"" + getValue(valName, nameSpace)->getDescription() +
          "\"/>");
}

std::string MinVRDataIndex::serialize(const std::string valName,
                                      const std::string nameSpace) {
  MinVRDatumPtr dataPtr = getValue(valName, nameSpace);




  return serialize(valName);
}


std::string MinVRDataIndex::serialize(const std::string valName) {
  MinVRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {
    throw std::runtime_error(std::string("never heard of ") + valName);
  } else {

    // This separates the valName on the slashes and puts the last
    // part of it into trimName.
    std::stringstream ss(valName);
    std::string trimName;
    while (std::getline(ss, trimName, '/')) {};

    // If this is not a container, just spell out the XML with the serialized
    // data inside.
    if (it->second->getType() != MVRCONTAINER) {

      return "<" + trimName + " type=\"" + it->second->getDescription() + "\">" +
        it->second->serialize() + "</" + trimName + ">";

    } else {
      // If this is a container...

      std::string serialized;
      //                      ... open the XML tag, with the type ...
      serialized = "<" + trimName + " type=\"" + it->second->getDescription() + "\">";

      // ... loop through the children (recursively) ...
      std::list<std::string> nameList = it->second.containerVal()->getValue();
      for (std::list<std::string>::iterator lt = nameList.begin();
           lt != nameList.end(); lt++) {

        // ... recurse, and get the serialization of the member data value.
        serialized += serialize(*lt);
      };

      serialized += "</" + trimName + ">";
      return serialized;
    }
  }
}

// an int should be <nWindows type="int">6</nWindows>
bool MinVRDataIndex::addValue(const std::string serializedData) {
  Cxml *xml = new Cxml();
  xml->parse_string((char*)serializedData.c_str());
  element *xml_node = xml->get_root_element();
  element* child = xml_node->get_next_child();

  while (child != NULL) {

    printXML(child, std::string(""));
    walkXML(child, std::string(""));

    child = xml_node->get_next_child();
  }

  delete xml;
}

bool MinVRDataIndex::addValue(const std::string serializedData,
                              const std::string nameSpace) {
  return addValue(serializedData);
}

bool MinVRDataIndex::processValue(const char* name,
                                  MVRTYPE_ID type,
                                  const char* valueString) {
  char buffer[50];

  //  std::cout << "Processing " << std::string(name) << " of type " << type << " with value (" << std::string(valueString) << ")" << std::endl;

  // Step 7 of adding a data type is adding entries to this switch.
  switch (type) {
  case MVRINT:
    {
      int iVal;
      sscanf(valueString, "%d", &iVal);

      //std::cout << "adding int " << std::string(name) << std::endl;
      addValueInt(name, iVal);
      break;
    }
  case MVRFLOAT:
    {
      double fVal;
      sscanf(valueString, "%lf", &fVal);

      //std::cout << "adding float " << std::string(name) << std::endl;
      addValueDouble(name, fVal);
      break;
    }
  case MVRSTRING:
    {
      std::string sVal = std::string(valueString);

      //std::cout << "adding string " << std::string(name) << std::endl;
      addValueString(name, sVal);
      break;
    }
  case MVRCONTAINER:
    {
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
  }
}

// This seems to read containers twice.  Do both instances wind up in memory?
bool MinVRDataIndex::walkXML(element* node, std::string nameSpace) {

  char type[5] = "type";

  std::string qualifiedName;
  std::list<std::string> childNames;

  qualifiedName = nameSpace + "/" + std::string(node->get_name());

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

      //std::cout << node->get_name() << ">>" << node->get_value() << "<<" << firstChar << "<<" << strlen(node->get_name()) << std::endl;

      // if (node->get_attribute(type) != NULL) {
      //   std::cout << ">>" << node->get_attribute(type)->get_value() << "<<" << std::endl;
      // } else {
      //   std::cout << ">><<" << std::endl;
      // }

      //if (node->get_value() != "") {
      if (firstChar >= 0) {

        MVRTYPE_ID typeId;

        if (node->get_attribute(type) == NULL) {
          //std::cout << "inferring type for: " << node->get_name() << std::endl;
          typeId = inferType(std::string(node->get_value()));
        } else { // what does map return if no match?
          typeId = mvrTypeMap[std::string(node->get_attribute(type)->get_value())];
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
        //std::cout << "adding CONTAINER" << std::endl;
        addValueContainer(qualifiedName, childNames);
      }
      return true;
    }

    // Collect a child name on the container's child name list.
    childNames.push_back(qualifiedName + "/" + child->get_name());

    // And go walk its tree.
    walkXML(child, qualifiedName);
  }
}

MVRTYPE_ID MinVRDataIndex::inferType(const std::string valueString) {

  // Test for int
  char *p;
  int conInt = strtol(valueString.c_str(), &p, 10);
  if (!*p) return MVRINT;

  //std::cout << "not an int" << std::endl;

  double conFloat = strtod(valueString.c_str(), &p);
  if (!*p) return MVRFLOAT;

  //std::cout << "not a float" << std::endl;

  // Is it a container?
  std::size_t firstChar = valueString.find_first_not_of(" \t\r\n");
  if (firstChar != std::string::npos) {
    if (valueString[firstChar] == '<') return MVRCONTAINER;
  }

  //std::cout << "not a container" << std::endl;

  // Not any of the above?  Probably a string.
  return MVRSTRING;
}

bool MinVRDataIndex::printXML(element* node, std::string prefix) {

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

bool MinVRDataIndex::processXMLFile(std::string fileName) {

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

      printXML(child, std::string(""));
      walkXML(child, std::string(""));

      child = xml_node->get_next_child();
    }

    delete xml;

  } else {
    std::cout << "Error opening file " << fileName << std::endl;
  }
}



