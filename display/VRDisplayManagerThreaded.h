#ifndef VRDISPLAYMANAGERTHREADED_H
#define VRDISPLAYMANAGERTHREADED_H

#include "VRDisplayManager.h"

/** Placeholder...
*/
class VRDisplayManagerThreaded : public VRDisplayManager {
public:

  VRDisplayManagerThreaded(std::vector<DisplayDevice*> displayDevices;);
  virtual ~VRDisplayManagerThreaded();

  virtual void renderFrame(void (*renderCallback)(const VRRenderState &));

  virtual void swapBuffers();

protected:

};

#endif
