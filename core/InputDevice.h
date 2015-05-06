#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <vector>
#include "VREvent.h"

/** InputDevice:
    An as-simple-as-possible public interface for Input Devices
 */
class InputDevice {
public:
  virtual void pollForInput(std::vector<VREvent*> &inputEvents);
};


#endif
