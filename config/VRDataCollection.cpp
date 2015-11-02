#include "VRDataCollection.h"

// Step 7 of the specialization instructions (in VRDatum.h) is to
// add an entry here to register the new data type.
VRDataCollection::VRDataCollection() {
  factory.RegisterVRDatum(MVRINT, CreateVRDatumInt);
  factory.RegisterVRDatum(MVRDOUBLE, CreateVRDatumDouble);
  factory.RegisterVRDatum(MVRSTRING, CreateVRDatumString);
  factory.RegisterVRDatum(MVRARRAYDOUBLE, CreateVRDatumArrayDouble);
  factory.RegisterVRDatum(MVRCONTAINER, CreateVRDatumContainer);

  // We create a VRDatum object here just to have access to the
  // typemap that is a static member of that class.  Copy it into a
  // map<> for use over here in reading serialized data strings.
  VRDatumInt *m = new VRDatumInt(0);
  for (int i = 0; i < MVRNTYPES; i++) {
    mvrTypeMap[std::string(m->MVRTypeMap[i].first)] = m->MVRTypeMap[i].second;
  }
}

std::string VRDataCollection::serialize(const std::string trimName,
                                           VRDatumPtr pdata ) {
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

int VRDataCollection::deserializeInt(const char* valueString) {
  int iVal;
  sscanf(valueString, "%d", &iVal);

  return iVal;
}

double VRDataCollection::deserializeDouble(const char* valueString) {
  double fVal;
  sscanf(valueString, "%lf", &fVal);

  return fVal;
}

std::string VRDataCollection::deserializeString(const char* valueString) {
  return std::string(valueString);
}

MVRArrayDouble VRDataCollection::deserializeArrayDouble(const char* valueString) {

  MVRArrayDouble vVal;

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

bool VRDataCollection::processValue(const char* name,
                                       MVRTYPE_ID type,
                                       const char* valueString) {
  char buffer[50];

  /// Step 9 of adding a data type is adding entries to this switch.
  /// And the corresponding deserialize*() method.
  switch (type) {
  case MVRINT:
    addValue(name, deserializeInt(valueString));
    break;

  case MVRDOUBLE:
    addValue(name, deserializeDouble(valueString));
    break;

  case MVRSTRING:
    addValue(name, deserializeString(valueString));
    break;

  case MVRARRAYDOUBLE:
    addValue(name, deserializeArrayDouble(valueString));
    break;

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
  case MVRNONE:
  case MVRARRAYINT:
  case MVRARRAYSTRING:
    {
      break;
    }	     
  }
  return true;
}

// This seems to read containers twice.  Do both instances wind up in memory?
bool VRDataCollection::walkXML(element* node, std::string nameSpace) {

  char type[5] = "type";

  std::string qualifiedName;
  MVRContainer childNames;

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

        addValue(qualifiedName, childNames);
      }
      return true;
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
MVRTYPE_ID VRDataCollection::inferType(const std::string valueString) {
/// Step 10 -- Add some functionality to this method to help identify
/// your new data type.

  // Test for int
  char *p;
  int conInt = strtol(valueString.c_str(), &p, 10);
  if (!*p) return MVRINT;

  double conDouble = strtod(valueString.c_str(), &p);
  if (!*p) return MVRDOUBLE;

  // Is it a container?
  std::size_t firstChar = valueString.find_first_not_of(" \t\r\n");
  if (firstChar != std::string::npos) {
    if (valueString[firstChar] == '<') return MVRCONTAINER;
  }

  // Not any of the above?  Probably a string.
  return MVRSTRING;
}

bool VRDataCollection::printXML(element* node, std::string prefix) {

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

