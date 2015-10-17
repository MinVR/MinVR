#include "MinVRDataCollection.h"

// Step 5 of the specialization instructions (in MinVRDatum.h) is to
// add an entry here to register the new data type.
MinVRDataCollection::MinVRDataCollection() {
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
  mvrTypeMap[std::string("MVRContainer")] =  MVRCONTAINER;

}

std::string MinVRDataCollection::serialize(const std::string trimName,
                                           MinVRDatumPtr pdata ) {
  // If this is not a container, just spell out the XML with the serialized
  // data inside.
  if (pdata->getType() != MVRCONTAINER) {

    return "<" + trimName + " type=\"" + pdata->getDescription() + "\">" +
      pdata->serialize() + "</" + trimName + ">";

  } else {
    // If this is a container...

    std::string serialized;
    //                      ... open the XML tag, with the type ...
    serialized = "<" + trimName + " type=\"" + pdata->getDescription() + "\">";

    // ... loop through the children (recursively) ...
    MVRContainer nameList = pdata->getValue();
    for (MVRContainer::iterator lt = nameList.begin();
         lt != nameList.end(); lt++) {

      // ... recurse, and get the serialization of the member data value.
      serialized += serialize(*lt);
    };

    serialized += "</" + trimName + ">";
    return serialized;
  }
}

bool MinVRDataCollection::processValue(const char* name,
                                       MVRTYPE_ID type,
                                       const char* valueString) {
  char buffer[50];

  // Step 7 of adding a data type is adding entries to this switch.
  switch (type) {
  case MVRINT:
    {
      int iVal;
      sscanf(valueString, "%d", &iVal);

      addValueInt(name, iVal);
      break;
    }
  case MVRFLOAT:
    {
      double fVal;
      sscanf(valueString, "%lf", &fVal);

      addValueDouble(name, fVal);
      break;
    }
  case MVRSTRING:
    {
      std::string sVal = std::string(valueString);

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
bool MinVRDataCollection::walkXML(element* node, std::string nameSpace) {

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

      if (firstChar >= 0) {

        MVRTYPE_ID typeId;

        if (node->get_attribute(type) == NULL) {

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

MVRTYPE_ID MinVRDataCollection::inferType(const std::string valueString) {

  // Test for int
  char *p;
  int conInt = strtol(valueString.c_str(), &p, 10);
  if (!*p) return MVRINT;

  double conFloat = strtod(valueString.c_str(), &p);
  if (!*p) return MVRFLOAT;

  // Is it a container?
  std::size_t firstChar = valueString.find_first_not_of(" \t\r\n");
  if (firstChar != std::string::npos) {
    if (valueString[firstChar] == '<') return MVRCONTAINER;
  }

  // Not any of the above?  Probably a string.
  return MVRSTRING;
}

bool MinVRDataCollection::printXML(element* node, std::string prefix) {

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

