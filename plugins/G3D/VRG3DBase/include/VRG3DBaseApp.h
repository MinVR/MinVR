

#ifndef VRG3DBASEVRAPP_H
#define VRG3DBASEVRAPP_H

#include "../../VRG3D/include/VRG3DApp.h"
#include "GfxMgr.H"
#include "EventMgr.H"

namespace MinVR
{

  class VRG3DBaseApp : public VRG3DApp
  {
  public:
    PLUGIN_API VRG3DBaseApp(int argc, char *argv[]);
    PLUGIN_API virtual ~VRG3DBaseApp();
    //PLUGIN_API virtual void onRenderGraphicsContext(const VRGraphicsState& state);
    PLUGIN_API virtual void onRenderGraphicsScene(const VRGraphicsState& state);

  protected:
    
    GfxMgrRef         _gfxMgr;
    G3D::Color4                _clearColor;
    MinVR::EventMgrRef       _eventMgr;
    //MouseToTrackerRef _mouseToTracker;


    
    
  };
}




#endif // !VRG3DBASEAPP




