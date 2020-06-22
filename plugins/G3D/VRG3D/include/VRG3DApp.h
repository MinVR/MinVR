

#ifndef VRG3DVRAPP_H
#define VRG3DVRAPP_H

#include <api/MinVR.h>

#include <G3D/G3D.h>
#include <GLG3D/GLG3D.h>
#include <G3D/G3DAll.h>
#include "../include/ProjectionVRCamera.h"

namespace MinVR
{

  class VRG3DApp : public VRApp
  {
  public:
    PLUGIN_API VRG3DApp(int argc, char *argv[]);
    PLUGIN_API virtual ~VRG3DApp();
    PLUGIN_API virtual void onRenderGraphicsContext(const VRGraphicsState& state);
    PLUGIN_API virtual void onRenderGraphicsScene(const VRGraphicsState& state);
    PLUGIN_API virtual void onCursorMove(const VRCursorEvent &state);
    PLUGIN_API virtual void onAnalogChange(const VRAnalogEvent &state);
    PLUGIN_API virtual void onButtonDown(const VRButtonEvent &state);
    PLUGIN_API virtual void onButtonUp(const VRButtonEvent &state);
    PLUGIN_API virtual void onTrackerMove(const VRTrackerEvent &state);


  protected:
    G3D::RenderDevice* myRenderDevice;
    G3D::Array<ProjectionVRCameraRef> _cameras;
    G3D::Log*                  _log;

    
    
  };
}




#endif // !VRG3DVRAPP




