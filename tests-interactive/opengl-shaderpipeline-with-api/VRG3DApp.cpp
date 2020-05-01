#include "VRG3DApp.h"
#include <VRG3DGraphicsToolkit.h>
#include <VRG3DDisplayNode.h>

VRG3DApp::VRG3DApp(int argc, char *argv[]):
  myRenderDevice(nullptr),
  MinVR::VRApp(argc, argv)
{

}

VRG3DApp::~VRG3DApp()
{

}

void VRG3DApp::onRenderGraphicsContext(const MinVR::VRGraphicsState& state)
{
  if (state.isInitialRenderCall()) {
    std::vector<MinVR::VRDisplayNode*> g3dDisplayNode =
      //getDisplayNodesByName("VRG3DGraphicsWindowNode");
      //getDisplayNodesByName("/MinVR/Desktop/WindowNode");
      getDisplayNodesByName("/MinVR/VRSetups/Desktop/WindowNode");
    

    //std::vector<MinVR::VRDisplayNode*> displayNodes = getDisplayNodesByType();

    for (int i  = 0 ; i< g3dDisplayNode.size();++i)
    {
      std::cout << g3dDisplayNode[i]->getType() << std::endl;
      MinVR::VRG3DDisplayNode* g3dGraphicsTK = dynamic_cast<MinVR::VRG3DDisplayNode*>(g3dDisplayNode[i]);
      if (g3dGraphicsTK)
      {
        G3D::RenderDevice* g3dRenderDevice = g3dGraphicsTK->getG3DRenderDevice();
        if (g3dRenderDevice)
        {
          std::cout << "SUCCESS " << std::endl;
          myRenderDevice = g3dRenderDevice;
          //loadScene();
        }
        else
        {
          std::cout << "FAIL" << std::endl;
        }
      }

      
    }
  }
}

void VRG3DApp::onRenderGraphicsScene(const MinVR::VRGraphicsState& state)
{
  //GLCaps::hasBug_redBlueMipmapSwap();
  //const float* pMatrix = state.getProjectionMatrix();
  G3D::Matrix4 projectionMtrx = state.getProjectionMatrix();
  projectionMtrx = projectionMtrx.transpose();
  const float* vMatrix = state.getViewMatrix();
  G3D::Matrix4 viewMatrix = state.getViewMatrix();
  viewMatrix = viewMatrix.transpose();
  G3D::Matrix4 g3dPjMtx = myRenderDevice->projectionMatrix();
  myRenderDevice->setProjectionMatrix(projectionMtrx);
  G3D::CoordinateFrame coorF = viewMatrix.approxCoordinateFrame();
  myRenderDevice->setCameraToWorldMatrix(viewMatrix.approxCoordinateFrame().inverse());


  G3D::Draw::sphere(G3D::Sphere(
    G3D::Vector3(0.0f, 0.0f, -1.0f), 0.5f), myRenderDevice, G3D::Color3::blue(), G3D::Color4::clear());
}

void VRG3DApp::loadScene()
{
//  GLCaps::hasBug_redBlueMipmapSwap();
  std::string sceneName = "Crates";
  // Load the scene
  //GLCaps::init();
  m_scene = Scene::create(sceneName);
}
