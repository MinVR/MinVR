#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <vector>
#include "VREvent.h"

/** InputDevice:
    An as-simple-as-possible public interface for Input Devices
 */
class InputDevice {
public:
  virtual void appendNewInputEventsSinceLastCall(std::vector<VREvent> &inputEvents) = 0;
};


#endif
