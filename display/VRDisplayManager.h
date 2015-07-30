#ifndef VRDISPLAYMANAGER_H
#define VRDISPLAYMANAGER_H

#include "VRDisplayDevice.h"
#include "VRRenderState.h"

/** This default implementaiton is single threaded, but the main purpose of having the VRDisplayManager
    abstraction is to enable a multi-threaded display manager, where each DisplayDevice is rendered in
    parallel in a separate thread.
*/
class VRDisplayManager {
public:
  VRDisplayManager(std::vector<DisplayDevice*> displayDevices;);
  virtual ~VRDisplayManager();

  virtual void handleUserInput(const std::vector<VREvent> &inputEvents);

  virtual void renderFrame(void (*renderCallback)(const VRRenderState &));

  virtual void swapBuffers();

protected:
  std::vector<DisplayDevice*> _displayDevices;
};

#endif
