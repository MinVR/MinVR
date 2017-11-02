#include "VRDatum.h"

namespace MinVR {

// This is the canonical list of how to spell the types given here.
// The strings here will appear in the XML serialization, in the
// 'type="XX"' part.  So you can change them here, and these changes
// will be reflected throughout the code, but not in any config files
// that use them.  This is part of step 1 of adding a new type.
VRDatum::VRTypeMap VRDatum::initializeTypeMap() {
  VRDatum::VRTypeMap newTypeMap;

  newTypeMap["none"] = VRCORETYPE_NONE;
  newTypeMap["int"] = VRCORETYPE_INT;
  newTypeMap["float"] = VRCORETYPE_FLOAT;
  newTypeMap["string"] = VRCORETYPE_STRING;
  newTypeMap["intarray"] = VRCORETYPE_INTARRAY;
  newTypeMap["floatarray"] = VRCORETYPE_FLOATARRAY;
  newTypeMap["stringarray"] = VRCORETYPE_STRINGARRAY;
  newTypeMap["container"] = VRCORETYPE_CONTAINER;

  return newTypeMap;
};

VRDatum::VRTypeMap VRDatum::typeMap = VRDatum::initializeTypeMap();

  // The constructor for the native storage form.
VRDatum::VRDatum(const VRCORETYPE_ID inType) : type(inType) {

  // Store an empty attribute list.
  attrList.push_front(VRAttributeList());

  // Do a reverse lookup on the typeMap to get the text description of
  // the input datum type.
  for (VRTypeMap::iterator it = VRDatum::typeMap.begin();
       it != VRDatum::typeMap.end(); it++) {
    if (it->second == inType) {
      description = it->first;
    }
  }
};

// Returns the attribute list formatted to include in an XML tag.
std::string VRDatum::getAttributeListAsString() const {
  std::string out = std::string("");
  for (VRAttributeList::const_iterator it = attrList.front().begin();
       it != attrList.front().end(); it++) {
    out += " " + it->first + "=\"" + it->second + "\"";
  }
  return out;
}


/// Step 4 in the adding a type instructions.
//////////////////////////////////////////// VRInt
std::string VRDatumInt::getValueString() const {
  char buffer[20];
  sprintf(buffer, "%d", value.front());
  return std::string(buffer);
}

VRDatumPtr CreateVRDatumInt(void *pData) {
  VRDatumInt *obj = new VRDatumInt(*static_cast<VRInt *>(pData));
  return VRDatumPtr(obj);
}

//////////////////////////////////////////// VRFloat
std::string VRDatumFloat::getValueString() const {
  char buffer[20];
  sprintf(buffer, "%f", value.front());
  return std::string(buffer);
}

VRDatumPtr CreateVRDatumFloat(void *pData) {
  VRDatumFloat *obj = new VRDatumFloat(*static_cast<VRFloat *>(pData));
  return VRDatumPtr(obj);
}

//////////////////////////////////////////// VRString
VRDatumPtr CreateVRDatumString(void *pData) {
  VRDatumString *obj = new VRDatumString(*static_cast<VRString *>(pData));
  return VRDatumPtr(obj);
}

//////////////////////////////////////////// VRIntArray
std::string VRDatumIntArray::getValueString() const {

  std::string out;
  char buffer[20];
  char separator;

  VRAttributeList::const_iterator it = attrList.front().find("separator");
  if (it == attrList.front().end()) {
    separator = MINVRSEPARATOR;
  } else {
    separator = static_cast<char>(it->second[0]);
  }

  for (VRIntArray::const_iterator it = value.front().begin();
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

//////////////////////////////////////////// VRFloatArray
std::string VRDatumFloatArray::getValueString() const {

  std::string out;
  char buffer[20];
  char separator;

  VRAttributeList::const_iterator it = attrList.front().find("separator");
  if (it == attrList.front().end()) {
    separator = MINVRSEPARATOR;
  } else {
    separator = static_cast<char>(it->second[0]);
  }

  for (VRFloatArray::const_iterator it = value.front().begin();
       it != value.front().end(); ++it) {
    sprintf(buffer, "%f%c", *it, separator);
    out += std::string(buffer);
  }

  // Remove trailing separator.
  return out.substr(0, out.size() - 1);
}

VRDatumPtr CreateVRDatumFloatArray(void *pData) {
  VRDatumFloatArray *obj =
    new VRDatumFloatArray(*static_cast<VRFloatArray *>(pData));
  return VRDatumPtr(obj);
}

//////////////////////////////////////////// VRStringArray
std::string VRDatumStringArray::getValueString() const {
  std::string out;
  char separator;

  VRAttributeList::const_iterator it = attrList.front().find("separator");
  if (it == attrList.front().end()) {
    separator = MINVRSEPARATOR;
  } else {
    separator = static_cast<char>(it->second[0]);
  }

  for (VRStringArray::const_iterator it = value.front().begin();
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

// This getValueString function for a container probably does not
// do what you think it does.  Use the serialize() function of the
// VRDataIndex for that.  This function just returns a formatted list
// of the strings that make up a container.
std::string VRDatumContainer::getValueString() const {
  std::string out;
  char separator;

  VRAttributeList::const_iterator it = attrList.front().find("separator");
  if (it == attrList.front().end()) {
    separator = MINVRSEPARATOR;
  } else {
    separator = static_cast<char>(it->second[0]);
  }

  for (VRContainer::const_iterator it = value.front().begin();
       it != value.front().end(); ++it) {
    out += *it + std::string(1,separator);
  }

  // Remove trailing separator.
  return out.substr(0, out.size() - 1);
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


//  Implemented for completeness sake.
std::ostream & operator<<(std::ostream &os, const VRDatum& p) {
  return os << p.getValueString();
}

std::ostream & operator<<(std::ostream &os, VRDatumPtr& p) {
  return os << p->getValueString();
}

} // end namespace MinVR
