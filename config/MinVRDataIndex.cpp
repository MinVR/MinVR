#include "MinVRDatum.h"
#include "MinVRDatumFactory.h"
#include "MinVRDataIndex.h"

// Step 5 of the specialization instructions (in MinVRDatum.h) is to
// add an entry here to register the new data type.
MinVRDataIndex::MinVRDataIndex() {
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

  MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRINT, &value);
  return mindex.insert(MinVRDataMap::value_type(valName, obj)).second;
}

bool MinVRDataIndex::addValueDouble(const std::string valName, double value) {

  MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRFLOAT, &value);
  //std::cout << "added " << obj.doubleVal()->getValue() << std::endl;
  return mindex.insert(MinVRDataMap::value_type(valName, obj)).second;
}

bool MinVRDataIndex::addValueString(const std::string valName, std::string value) {

  MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRSTRING, &value);
  //std::cout << "added " << obj.stringVal()->getValue() << std::endl;
  return mindex.insert(MinVRDataMap::value_type(valName, obj)).second;
}

bool MinVRDataIndex::addValueContainer(const std::string valName,
                                       std::list<std::string> value) {

  MinVRDatumPtr obj = factory.CreateMinVRDatum(MVRCONTAINER, &value);
  //std::cout << "added " << obj.containerVal()->getValue() << std::endl;
  return mindex.insert(MinVRDataMap::value_type(valName, obj)).second;
}

MinVRDatumPtr MinVRDataIndex::getValue(const std::string valName) {
  MinVRDataMap::const_iterator it = mindex.find(valName);
  if (it == mindex.end()) {
    throw std::runtime_error(std::string("never heard of ") + valName);
  } else {
    return it->second;
  }
}

std::string MinVRDataIndex::getDescription(const std::string valName) {
  MinVRDataMap::iterator it = mindex.find(valName);
  if (it == mindex.end()) {
    throw std::runtime_error(std::string("never heard of ") + valName);
  } else {
    return it->second->getDescription() + " " + valName + ";";
  }
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
// A container: <section id="myContainer"><nWindows type="int">6</nWindows>
bool MinVRDataIndex::addValue(const std::string serializedData) {
  Cxml *xml = new Cxml();
  xml->parse_string((char*)serializedData.c_str());
  element *xml_node = xml->get_root_element();

  walkXML(xml_node, std::string("XML_DOC"));
}

bool MinVRDataIndex::addValue(const std::string serializedData,
                              const std::string nameSpace) {
  return addValue(serializedData);
}

bool MinVRDataIndex::processValue(const char* name,
                                  const char* type,
                                  const char* valueString) {
  char buffer[50];

  std::cout << "Processing " << std::string(name) << " of type " << std::string(type) << " with value (" << std::string(valueString) << ")" << std::endl;

  // Step 7 of adding a data type is adding entries to this switch.
  switch (mvrTypeMap[std::string(type)]) {
  case MVRINT:
    {
      int iVal;
      sscanf(valueString, "%d", &iVal);

      std::cout << "adding int " << std::string(name) << std::endl;
      addValueInt(name, iVal);
      break;
    }
  case MVRFLOAT:
    {
      double fVal;
      sscanf(valueString, "%lf", &fVal);

      std::cout << "adding float " << std::string(name) << std::endl;
      addValueDouble(name, fVal);
      break;
    }
  case MVRSTRING:
    {
      std::string sVal = std::string(valueString);

      std::cout << "adding string " << std::string(name) << std::endl;
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

//bool processValue(name, type, processedXMLNode);


bool MinVRDataIndex::walkXML(element* node, std::string nameSpace) {

  char type[5] = "type";

  std::string qualifiedName;
  std::list<std::string> childNames;

  // The qualified name is the nameSpace/nodeName.  The root node is
  // always 'XML_DOC' so we ignore that one.
  if (!nameSpace.compare("XML_DOC")) {
    qualifiedName = std::string(node->get_name());
  } else {
    qualifiedName = nameSpace + "/" + std::string(node->get_name());
  }

  // This loops through the node children, if there are any.
  while (true) {

    // If there is a value, submit this node to processValue.
    // Container nodes should not be processed this way because they
    // have children, not a value.  Or at least they should not, and
    // the processValue method will throw an exception.
    if (node->get_value() != NULL &&
        node->get_value() != "") {

      processValue(qualifiedName.c_str(),
                   node->get_attribute(type)->get_value(),
                   node->get_value());

    }

    // Pick the next child.
    element* child = node->get_next_child();
    if (child == NULL) {

      // If this is a non-empty container that is not named XML_DOC,
      // add it to the index.
      if (childNames.size() > 0 && strcmp(node->get_name(), "XML_DOC")) {

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

bool MinVRDataIndex::printXML(element* node, int level) {

  char type[5] = "type";
  std::cout << node->get_name();
  if (node->get_attribute(type)) {
    cout << " (" << node->get_attribute(type)->get_name() << ") ";
  }

  while (true) {

    if (node->get_value() != NULL &&
        node->get_value() != "") {

      std::cout << node->get_value() << std::endl;
    } else {
      std::cout << std::endl;
    }
    element* child = node->get_next_child();
    if (child == NULL) {

      return true;
    }

    printXML(child, level + 1);
  }
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
    walkXML(xml_node, std::string("XML_DOC"));
    delete xml;

  } else {
    std::cout << "Error opening file " << fileName << std::endl;
  }
}



