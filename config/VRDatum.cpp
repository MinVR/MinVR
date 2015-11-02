#include "VRDatum.h"

// This is the canonical list of how to spell the types given here.
// The strings here will appear in the XML serialization, in the
// 'type="XX"' part.  So you can change them here, and these changes
// will be reflected throughout the code, but not in any config files
// that use them.  This is part of step 1 of adding a new type.
const VRDatum::MVRTypePair VRDatum::MVRTypeMap[MVRNTYPES] = {
  {"none", MVRNONE},
  {"int", MVRINT},
  {"double", MVRDOUBLE},
  {"string", MVRSTRING},
  {"arraydouble", MVRARRAYDOUBLE},
  {"container", MVRCONTAINER}
};

// This is just a convenience for initializing the description field
// in each object.  Note that it has no error checking, so get the
// MVRNTYPES correct, please.
std::string VRDatum::initializeDescription(MVRTYPE_ID t) {
  for (int i = 0; i < MVRNTYPES; i++) {
    if (MVRTypeMap[i].second == t) {
      return MVRTypeMap[i].first;
    }
  }
  return MVRTypeMap[0].first; // Should never reach here.
}


VRDatumInt::VRDatumInt(const int inVal) :
  VRDatum(MVRINT), value(inVal) {
  description = initializeDescription(type);
};

/// Step 4 in the adding a type instructions.
bool VRDatumInt::setValue(const int inVal) {
  value = inVal;
  return true;
}

std::string VRDatumInt::serialize() {
  char buffer[20];
  sprintf(buffer, "%d", value);
  return std::string(buffer);
}

VRDatumPtr CreateVRDatumInt(void *pData) {
  VRDatumInt *obj = new VRDatumInt(*static_cast<int *>(pData));
  return VRDatumPtr(obj);
}

////////////////////////////////////////////

VRDatumDouble::VRDatumDouble(const double inVal) :
  VRDatum(MVRDOUBLE), value(inVal) {
  description = initializeDescription(type);
};

bool VRDatumDouble::setValue(const double inVal) {
  value = inVal;
  return true;
}

std::string VRDatumDouble::serialize() {
  char buffer[20];
  sprintf(buffer, "%f", value);
  return std::string(buffer);
}

VRDatumPtr CreateVRDatumDouble(void *pData) {
  VRDatumDouble *obj = new VRDatumDouble(*static_cast<double *>(pData));
  return VRDatumPtr(obj);
}

////////////////////////////////////////////

VRDatumString::VRDatumString(const std::string inVal) :
  VRDatum(MVRSTRING), value(inVal) {
  description = initializeDescription(type);
};

bool VRDatumString::setValue(const std::string inVal) {
  value = inVal;
  return true;
}

std::string VRDatumString::serialize() {
  return value;
}

VRDatumPtr CreateVRDatumString(void *pData) {
  VRDatumString *obj = new VRDatumString(*static_cast<std::string *>(pData));
  return VRDatumPtr(obj);
}

////////////////////////////////////////////

VRDatumArrayDouble::VRDatumArrayDouble(const std::vector<double> inVal) :
  VRDatum(MVRARRAYDOUBLE), value(inVal) {
  description = initializeDescription(type);
};

bool VRDatumArrayDouble::setValue(const std::vector<double> inVal) {
  value = inVal;
  return true;
}

std::string VRDatumArrayDouble::serialize() {

  std::string out;
  char buffer[20];

  for (MVRArrayDouble::iterator it = value.begin(); it != value.end(); ++it) {
    sprintf(buffer, "%f@", *it); // '@' is a separator
    out += std::string(buffer);
  }

  return out;
}

VRDatumPtr CreateVRDatumArrayDouble(void *pData) {
  VRDatumArrayDouble *obj =
    new VRDatumArrayDouble(*static_cast<MVRArrayDouble *>(pData));
  return VRDatumPtr(obj);
}

////////////////////////////////////////////

VRDatumContainer::VRDatumContainer(const MVRContainer inVal) :
  VRDatum(MVRCONTAINER), value(inVal) {
  description = initializeDescription(type);
};

// For optimization and code maintainability reasons, the
// responsibility for assembling the serialization of a container
// falls to the index class, so this function is sort of a nop, filled
// out just to keep the compiler happy.
std::string VRDatumContainer::serialize() {
  throw std::runtime_error(std::string("shouldn't call the serialize() method of a container object."));
  return getDescription();
}

bool VRDatumContainer::addToValue(const MVRContainer inVal) {
  std::list<std::string> inCopy = inVal;

  // Remove all duplicates from the input list.
  for (MVRContainer::const_iterator it = value.begin();
       it != value.end(); ++it) {

    inCopy.remove(*it);

  }
  value.splice(value.end(), inCopy);
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
  VRDatumContainer *obj = new VRDatumContainer(*static_cast<MVRContainer *>(pData));
  return VRDatumPtr(obj);
}


//  We should probably implement this for completeness sake.
// std::ostream & operator<<(std::ostream &os, const VRDatum& p)
// {
//   return os << p.serialize();
// }
// std::ostream & operator<<(std::ostream &os, const VRDatumPtr& p)
// {
//   return os << p->serialize();
// }

