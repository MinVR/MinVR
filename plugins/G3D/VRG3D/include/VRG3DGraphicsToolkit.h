#ifndef VRG3DGRAPHICSTOOLKIT_H
#define VRG3DGRAPHICSTOOLKIT_H

#include <display/VRGraphicsToolkit.h>
#include <main/VRFactory.h>
#include <plugin/VRPlugin.h>
#include <G3D/G3D.h>
#include <GLG3D/GLG3D.h>

namespace MinVR {


  class G3D::RenderDevice;
  /** OpenGL Implementation for the MinVR VRGraphicsToolkit abstraction.
   */
  class VRG3DGraphicsToolkit : public VRGraphicsToolkit {
  public:
    PLUGIN_API VRG3DGraphicsToolkit(VRMainInterface *vrMain);
    PLUGIN_API virtual ~VRG3DGraphicsToolkit();

    PLUGIN_API std::string getName() const { return "VRG3DGraphicsToolkit"; }

    PLUGIN_API void setDrawBuffer(VRDRAWBUFFER buffer);
    //PLUGIN_API void setSubWindow(VRRect rect);
    PLUGIN_API void flushGraphics();
    PLUGIN_API void finishGraphics();

    PLUGIN_API static VRGraphicsToolkit* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

    void InitG3DRenderDevice(G3D::OSWindow*);
    G3D::RenderDevice* getG3DRenderDevice();

  private:
    G3D::int64                 _frameCounter;
    G3D::RenderDevice* _renderDevice;
    VRMainInterface *_vrMain;
    bool isInitialized;

  };


} // end namespace

#endif
