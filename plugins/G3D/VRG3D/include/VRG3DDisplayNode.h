
#ifndef VRG3GRAPHICSWINDOWNODE_H
#define VRG3GRAPHICSWINDOWNODE_H

#include <display/VRGraphicsWindowNode.h>
#include <plugin/VRPlugin.h>
#include <G3D/G3D.h>
#include <GLG3D/GLG3D.h>

class G3D::RenderDevice;

namespace MinVR {

  

  class VRG3DDisplayNode :public VRDisplayNode
  {
  public:

    PLUGIN_API VRG3DDisplayNode(const std::string &name, VRGraphicsToolkit *gfxToolkit, VRWindowToolkit *winToolkit, const VRWindowSettings &settings, int windowId);
    PLUGIN_API virtual ~VRG3DDisplayNode();
    PLUGIN_API virtual std::string getType() const { return "VRG3DGraphicsWindowNode"; }
    PLUGIN_API virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);
    PLUGIN_API static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);
    PLUGIN_API G3D::RenderDevice* getG3DRenderDevice();
    PLUGIN_API virtual void waitForRenderToComplete(VRDataIndex *renderState);
    PLUGIN_API virtual void displayFinishedRendering(VRDataIndex *renderState);

  private:

    int _windowID;
    VRGraphicsToolkit *_gfxToolkit;
    VRWindowToolkit   *_winToolkit;
    VRWindowSettings   _settings;
    int _framebufferWidth;
    int _framebufferHeight;

  };

}

#endif