
#ifndef VRG3DVRAPP_H
#define VRG3DVRAPP_H
#include <api/MinVR.h>
#include <G3D/G3D.h>
#include <GLG3D/GLG3D.h>
#include "Scene.h"

class MinVR::VRGraphicsState;

class VRG3DApp:public MinVR::VRApp
{
public:
  VRG3DApp(int argc, char *argv[]);
  ~VRG3DApp();
  void onRenderGraphicsContext(const MinVR::VRGraphicsState& state);
  void onRenderGraphicsScene(const MinVR::VRGraphicsState& state);
  void loadScene();

private:
  G3D::RenderDevice* myRenderDevice;
  Scene::Ref          m_scene;
};

#endif // !VRG3DVRAPP




