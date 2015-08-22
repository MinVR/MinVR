#include "MinVRDatumFactory.h"

MinVRDatumPtr MinVRDatumFactory::CreateMinVRDatum(int MinVRDatumId,
                                                      void* pData) {
  // try to find the callback corresponding to the given shape id;
  // if no shape id found, throw exception
  CallbackMap::const_iterator it = m_callbacks.find(MinVRDatumId);
  if (it == m_callbacks.end()) {
    throw std::runtime_error("unknown shape id");
  } else {
    // create the instance using the creator callback
    return MinVRDatumPtr((it->second)(static_cast<void *>(pData)));
  }
}

bool MinVRDatumFactory::RegisterMinVRDatum(int MinVRDatumId,
                                           CreateMinVRDatumCallback Creator) {
  // returns true if type was registered; false if it had already
  // been registered
  return m_callbacks.insert(CallbackMap::value_type(MinVRDatumId, Creator)).second;
}

bool MinVRDatumFactory::UnRegisterMinVRDatum(int MinVRDatumId) {
  // returns true if shape was unregistered, false if it was not
  // registered in the first place
  return m_callbacks.erase(MinVRDatumId) == 1;
}

