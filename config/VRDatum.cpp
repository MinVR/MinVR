#include "VRDatum.h"

// This is the canonical list of how to spell the types given here.
// The strings here will appear in the XML serialization, in the
// 'type="XX"' part.  So you can change them here, and these changes
// will be reflected throughout the code, but not in any config files
// that use them.  This is part of step 1 of adding a new type.
const VRDatum::VRTypePair VRDatum::VRTypeMap[VRCORETYPE_NTYPES] = {
  {"none", VRCORETYPE_NONE},
  {"int", VRCORETYPE_INT},
  {"double", VRCORETYPE_DOUBLE},
  {"string", VRCORETYPE_STRING},
  {"intarray", VRCORETYPE_INTARRAY},
  {"doublearray", VRCORETYPE_DOUBLEARRAY},
  {"stringarray", VRCORETYPE_STRINGARRAY},
  {"container", VRCORETYPE_CONTAINER}
};

// This is just a convenience for initializing the description field
// in each object.  Note that it has no error checking, so get the
// VRCORETYPE_NTYPES correct, please.
std::string VRDatum::initializeDescription(VRCORETYPE_ID t) {
  for (int i = 0; i < VRCORETYPE_NTYPES; i++) {
    if (VRTypeMap[i].second == t) {
      return VRTypeMap[i].first;
    }
  }
  return VRTypeMap[0].first; // Should never reach here.
}

// Returns the attribute list formatted to include in an XML tag.
std::string VRDatum::getAttributeListAsString() {
  std::string out = std::string("");
  for (VRAttributeList::iterator it = attrList.front().begin();
       it != attrList.front().end(); it++) {
    out += " " + it->first + "=\"" + it->second + "\"";
  }
  return out;
}


/// Step 4 in the adding a type instructions.
//////////////////////////////////////////// VRInt
std::string VRDatumInt::getValueAsString() {
  char buffer[20];
  sprintf(buffer, "%d", value.front());
  return std::string(buffer);
}

VRDatumPtr CreateVRDatumInt(void *pData) {
  VRDatumInt *obj = new VRDatumInt(*static_cast<VRInt *>(pData));
  return VRDatumPtr(obj);
}

//////////////////////////////////////////// VRDouble
std::string VRDatumDouble::getValueAsString() {
  char buffer[20];
  sprintf(buffer, "%f", value.front());
  return std::string(buffer);
}

VRDatumPtr CreateVRDatumDouble(void *pData) {
  VRDatumDouble *obj = new VRDatumDouble(*static_cast<VRDouble *>(pData));
  return VRDatumPtr(obj);
}

//////////////////////////////////////////// VRString
VRString VRDatumString::getValueAsString() {
  return value.front();
}

VRDatumPtr CreateVRDatumString(void *pData) {
  VRDatumString *obj = new VRDatumString(*static_cast<VRString *>(pData));
  return VRDatumPtr(obj);
}

//////////////////////////////////////////// VRIntArray
std::string VRDatumIntArray::getValueAsString() {

  std::string out;
  char buffer[20];
  char separator;

  VRAttributeList::iterator it = attrList.front().find("separator");
  if (it == attrList.front().end()) {
    separator = MINVRSEPARATOR;
  } else {
    separator = static_cast<char>(it->second[0]);
  }
  
  for (VRIntArray::iterator it = value.front().begin();
       it != value.front().end(); ++it) {
    sprintf(buffer, "%d%c", *it, separator); 

    out += std::string(buffer);
  }

  // Remove trailing separator.
  return out.substr(0, out.size() - 1);
}

VRDatumPtr CreateVRDatumIntArray(void *pData) {
  VRDatumIntArray *obj =
    new VRDatumIntArray(*static_cast<VRIntArray *>(pData));
  return VRDatumPtr(obj);
}

//////////////////////////////////////////// VRDoubleArray
std::string VRDatumDoubleArray::getValueAsString() {

  std::string out;
  char buffer[20];
  char separator;

  VRAttributeList::iterator it = attrList.front().find("separator");
  if (it == attrList.front().end()) {
    separator = MINVRSEPARATOR;
  } else {
    separator = static_cast<char>(it->second[0]);
  }

  for (VRDoubleArray::iterator it = value.front().begin();
       it != value.front().end(); ++it) {
    sprintf(buffer, "%f%c", *it, separator);
    out += std::string(buffer);
  }

  // Remove trailing separator.
  return out.substr(0, out.size() - 1);
}

VRDatumPtr CreateVRDatumDoubleArray(void *pData) {
  VRDatumDoubleArray *obj =
    new VRDatumDoubleArray(*static_cast<VRDoubleArray *>(pData));
  return VRDatumPtr(obj);
}

//////////////////////////////////////////// VRStringArray
std::string VRDatumStringArray::getValueAsString() {
  std::string out;
  char separator;

  VRAttributeList::iterator it = attrList.front().find("separator");
  if (it == attrList.front().end()) {
    separator = MINVRSEPARATOR;
  } else {
    separator = static_cast<char>(it->second[0]);
  }
  
  for (VRStringArray::iterator it = value.front().begin();
       it != value.front().end(); ++it) {
    out += *it + std::string(1,separator);
  }

  // Remove trailing separator.
  return out.substr(0, out.size() - 1);
}

VRDatumPtr CreateVRDatumStringArray(void *pData) {
  VRDatumStringArray *obj =
    new VRDatumStringArray(*static_cast<VRStringArray *>(pData));
  return VRDatumPtr(obj);
}

//////////////////////////////////////////// VRContainer

// For optimization and code maintainability reasons, the
// responsibility for assembling the serialization of a container
// falls to the index class, so this function is sort of a nop, filled
// out just to keep the compiler happy.
std::string VRDatumContainer::getValueAsString() {
  throw std::runtime_error(std::string("shouldn't call the getValueAsString() method of a container object."));
  return getDescription();
}

bool VRDatumContainer::addToValue(const VRContainer inVal) {
  VRContainer inCopy = inVal;

  // If we need to push a new container onto the stack, do it here.
  if (needPush) {
    value.push_front( value.front() );
    attrList.push_front( attrList.front() );
    needPush = false;
    pushed = true;
  }
  
  // Remove all duplicates from the input list.
  for (VRContainer::const_iterator it = value.front().begin();
       it != value.front().end(); ++it) {

    inCopy.remove(*it);
  }
  
  value.front().splice(value.front().end(), inCopy);
  return true;
}


// This simply removes an entry from a container.  The corresponding
// name should be removed from the VRDataIndex, but that's an
// independent step.  Note that there are no error checks, so get the
// name right before deleting.
// bool VRDatumContainer::removeValue(const std::string rmVal) {

//   value.remove(rmVal);
// }

VRDatumPtr CreateVRDatumContainer(void *pData) {
  VRDatumContainer *obj = new VRDatumContainer(*static_cast<VRContainer *>(pData));
  return VRDatumPtr(obj);
}


//  We should probably implement this for completeness sake.
// std::ostream & operator<<(std::ostream &os, const VRDatum& p)
// {
//   return os << p.getValueAsString();
// }
// std::ostream & operator<<(std::ostream &os, const VRDatumPtr& p)
// {
//   return os << p->getValueAsString();
// }

