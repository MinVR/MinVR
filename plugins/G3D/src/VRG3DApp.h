

#ifndef VRG3DVRAPP_H
#define VRG3DVRAPP_H

#include <api/MinVR.h>

#include <G3D/G3D.h>
#include <GLG3D/GLG3D.h>
#include <G3D/G3DAll.h>


namespace MinVR
{

  class VRG3DApp : public VRApp
  {
  public:
    PLUGIN_API VRG3DApp(int argc, char *argv[]);
    PLUGIN_API ~VRG3DApp();
    PLUGIN_API virtual void onRenderGraphicsContext(const VRGraphicsState& state);
    PLUGIN_API virtual void onRenderGraphicsScene(const VRGraphicsState& state);
 

  protected:
    G3D::RenderDevice* myRenderDevice;
    
    

    
    
  };
}




#endif // !VRG3DVRAPP




