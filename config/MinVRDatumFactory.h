#include <map>
#include <iostream>
#include <stdexcept>
#include "MinVRDatum.h"

// this factory architecture was suggested by Andrei Alexandrescu in
// his book "Modern C++ Design" --- read it to get the full
// explanation (and a more generic implementation); this is just an
// example

// This class is where we parse the serialized data and create the
// appropriate MinVRDatum to hold that information.

class MinVRDatumFactory {
public:
  // this typedef allows to "name" arbitrary functions which take no
  // arguments and return a pointer to a MinVRDatum instance
  typedef MinVRDatum* (*CreateMinVRDatumCallback)();

  MinVRDatum* CreateMinVRDatum(int MinVRDatumId);

  bool RegisterMinVRDatum(int MinVRDatumId,
                          CreateMinVRDatumCallback Creator);

  bool UnRegisterMinVRDatum(int MinVRDatumId);

private:
  // the typedef simplifies the implementation
  typedef std::map<int, CreateMinVRDatumCallback> CallbackMap;
  // the callbacks are stored in a map int->callback (see typedef
  // above)
  CallbackMap m_callbacks;
};


