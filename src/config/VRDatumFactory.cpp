#include "VRDatumFactory.h"

namespace MinVR {

VRDatumPtr VRDatumFactory::CreateVRDatum(int VRDatumId,
                                         void* pData) {
  // try to find the callback corresponding to the given type id;
  // if no id found, throw exception
  CallbackMap::const_iterator it = m_callbacks.find(VRDatumId);
  if (it == m_callbacks.end()) {
    throw std::runtime_error("unknown VR type id");
  } else {
    // create the instance using the creator callback
    return VRDatumPtr((it->second)(static_cast<void *>(pData)));
  }
}

bool VRDatumFactory::RegisterVRDatum(int VRDatumId,
                                           CreateVRDatumCallback Creator) {
  // returns true if type was registered; false if it had already
  // been registered
  return m_callbacks.insert(CallbackMap::value_type(VRDatumId, Creator)).second;
}

} // end namespace MinVR
