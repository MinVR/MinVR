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
    G3D::RenderDevice* g3dRenderDevice;
  protected:
    void appendToMinVREvents(G3D::Array<G3D::GEvent>, VRDataQueue* minvrEventQueue);
    std::vector<G3DWindow* > _windows;
    //std::vector<VRDataIndex> _gEvents;
    static std::string getKeyName(G3D::GKey::Value key, G3D::GKeyMod mod);
  };

}
#endif