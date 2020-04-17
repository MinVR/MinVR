#ifndef VRG3DINPUTDEVICE_H_
#define VRG3DINPUTDEVICE_H_

#include <input/VRInputDevice.h>
#include <plugin/VRPlugin.h>
#include <GLG3D/OSWindow.h>


namespace MinVR {

  class VRDataQueue;
  typedef G3D::OSWindow G3DWindow;
  
  class VRG3DInputDevice : public VRInputDevice
  {
  public:
    PLUGIN_API VRG3DInputDevice();
    PLUGIN_API virtual ~VRG3DInputDevice();
    PLUGIN_API void appendNewInputEventsSinceLastCall(VRDataQueue* queue);
    PLUGIN_API void addWindow(G3DWindow*);
  private:
    std::vector<G3DWindow* > _windows;

  };

}
#endif