#ifndef DISPLAYDEVICE_H
#define DISPLAYDEVICE_H

#include <vector>
#include "VREvent.h"
#include "VRRenderState.h"


/** DisplayDevice:
    An as-simple-as-possible public interface for Display Devices
 */
class DisplayDevice {
public:

  /// The typical use for this function is to update display settings (e.g., projection matrix) based on head tracking input
  virtual void handleUserInput(const std::vector<VREvent> &inputEvents) {}

  virtual int getNumRenderingPasses() { return 1; }
  virtual void startRenderingPass(int passNum, VRRenderState &state) {}
  virtual void endRenderingPass(int passNum, VRRenderState &state) {}

  virtual void swapBuffers() {}
};




#endif
