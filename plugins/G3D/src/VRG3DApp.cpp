
#include "VRG3DApp.h"
#include "VRG3DGraphicsToolkit.h"
#include "VRG3DDisplayNode.h"

namespace MinVR
{
  VRG3DApp::VRG3DApp(int argc, char *argv[]) :
    VRApp(argc, argv)
  {

  }

  VRG3DApp::~VRG3DApp()
  {

  }

  void VRG3DApp::onRenderGraphicsContext(const VRGraphicsState& state)
  {
    if (state.isInitialRenderCall()) {
      std::vector<MinVR::VRDisplayNode*> g3dDisplayNode =
        //getDisplayNodesByName("VRG3DGraphicsWindowNode");
        //getDisplayNodesByName("/MinVR/Desktop/WindowNode");
        getDisplayNodesByName("/MinVR/VRSetups/Desktop/WindowNode");


      for (int i  = 0 ; i< g3dDisplayNode.size();++i)
      {
        std::cout << g3dDisplayNode[i]->getType() << std::endl;
        MinVR::VRG3DDisplayNode* g3dGraphicsTK = dynamic_cast<MinVR::VRG3DDisplayNode*>(g3dDisplayNode[i]);
        if (g3dGraphicsTK)
        {
          G3D::RenderDevice* g3dRenderDevice = g3dGraphicsTK->getG3DRenderDevice();
          if (g3dRenderDevice)
          {
            myRenderDevice = g3dRenderDevice;
          }
          else
          {
            assert(false || "Could not find /MinVR/VRSetups/Desktop/WindowNode/VRG3DDisplayNode");
          }
        }


      }
    }
  }

  void VRG3DApp::onRenderGraphicsScene(const VRGraphicsState& state)
  {
    /*
      Set up Projection and Camera-World Matrix
    */
    G3D::Matrix4 projectionMtrx = state.getProjectionMatrix();
    projectionMtrx = projectionMtrx.transpose();
    const float* vMatrix = state.getViewMatrix();
    G3D::Matrix4 viewMatrix = state.getViewMatrix();
    viewMatrix = viewMatrix.transpose();
    viewMatrix.setRow(1, -viewMatrix.row(1));
    G3D::Matrix4 g3dPjMtx = myRenderDevice->projectionMatrix();
    myRenderDevice->setProjectionMatrix(projectionMtrx);
    myRenderDevice->setCameraToWorldMatrix(viewMatrix.approxCoordinateFrame().inverse());

  }


}

