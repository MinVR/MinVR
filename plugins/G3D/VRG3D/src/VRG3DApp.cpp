
#include "../include/VRG3DApp.h"
#include "../include/VRG3DGraphicsToolkit.h"
#include "../include/VRG3DDisplayNode.h"

#include <api/VRApp.h>

namespace MinVR
{
  VRG3DApp::VRG3DApp(int argc, char *argv[]) :
    VRApp(argc, argv), _log(NULL)
  {    

    ProjectionVRCamera::ViewConfiguration cameraViewConfiguration =
      ProjectionVRCamera::ViewConfiguration::DESKTOP;
      ;

    std::vector<MinVR::VRDisplayNode*> g3dDisplayNode =
      //getDisplayNodesByName("VRG3DGraphicsWindowNode");
      //getDisplayNodesByName("/MinVR/Desktop/WindowNode");
      getDisplayNodesByName("WindowNode");
    for (int i = 0; i < g3dDisplayNode.size(); ++i)
    {
      std::cout << g3dDisplayNode[i]->getType() << std::endl;
      MinVR::VRG3DDisplayNode* g3dGraphicsTK = dynamic_cast<MinVR::VRG3DDisplayNode*>(g3dDisplayNode[i]);
      if (g3dGraphicsTK)
      {
        // check display configuration

        for (VRDisplayNode* displayNode: g3dGraphicsTK->getChildren())
        {
          if (displayNode->getType() == "HTC")
          {
            cameraViewConfiguration = ProjectionVRCamera::ViewConfiguration::VR;
          }
        }

        G3D::RenderDevice* g3dRenderDevice = g3dGraphicsTK->getG3DRenderDevice();
        if (g3dRenderDevice)
        {
          myRenderDevice = g3dRenderDevice;
          if (!_log)
          {
            _log = new Log("MinVR_G3D_Log.txt");
          }
        }
        else
        {
          assert(false || "Could not find /MinVR/VRSetups/Desktop/WindowNode/VRG3DDisplayNode");
        }
      }


    }

    std::vector<VRDisplayNode*> projectionDisplayNode =
      getDisplayNodesByName("ProjectionNode");

    if (projectionDisplayNode.size() > 0)
    {
      VRProjectionNode* projectionNode = dynamic_cast<MinVR::VRProjectionNode*>(projectionDisplayNode[0]);
      if (projectionNode)
      {
        float degreeToRadian = 3.1415926f / 180;
        float fovX = projectionNode->getFoVX();
        float fovY = projectionNode->getFoVY();
        float nearClip = projectionNode->getNearClip();
        float farClip = projectionNode->getFarClip();
        float horizontalClip = tan(fovX * degreeToRadian / 2.0f) * nearClip;
        float verticalClip = tan(fovY * degreeToRadian / 2.0f) * nearClip;


        _cameras.append(new ProjectionVRCamera(-horizontalClip, horizontalClip, -verticalClip
          , verticalClip, nearClip, farClip, cameraViewConfiguration));
      }

    }
    else
    {
      _cameras.append(new ProjectionVRCamera(cameraViewConfiguration));
    }
  }

  VRG3DApp::~VRG3DApp()
  {

  }

  void VRG3DApp::onRenderGraphicsContext(const VRGraphicsState& state)
  {
    if (state.isInitialRenderCall()) {
      //std::vector<MinVR::VRDisplayNode*> g3dDisplayNode =
      //  //getDisplayNodesByName("VRG3DGraphicsWindowNode");
      //  //getDisplayNodesByName("/MinVR/Desktop/WindowNode");
      //  getDisplayNodesByName("/MinVR/VRSetups/Desktop/WindowNode");


      //for (int i  = 0 ; i< g3dDisplayNode.size();++i)
      //{
      //  std::cout << g3dDisplayNode[i]->getType() << std::endl;
      //  MinVR::VRG3DDisplayNode* g3dGraphicsTK = dynamic_cast<MinVR::VRG3DDisplayNode*>(g3dDisplayNode[i]);
      //  if (g3dGraphicsTK)
      //  {
      //    G3D::RenderDevice* g3dRenderDevice = g3dGraphicsTK->getG3DRenderDevice();
      //    if (g3dRenderDevice)
      //    {
      //      myRenderDevice = g3dRenderDevice;
      //      if (!_log)
      //      {
      //        _log = new Log("MinVR_G3D_Log.txt");
      //      }
      //    }
      //    else
      //    {
      //      assert(false || "Could not find /MinVR/VRSetups/Desktop/WindowNode/VRG3DDisplayNode");
      //    }
      //  }


      //}
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
    viewMatrix.setColumn(1, -viewMatrix.row(1));
    viewMatrix.setColumn(3, G3D::Vector4(0,-1,0,0));
    G3D::Matrix4 g3dPjMtx = myRenderDevice->projectionMatrix();
    myRenderDevice->setProjectionMatrix(projectionMtrx);
    myRenderDevice->setCameraToWorldMatrix(viewMatrix.approxCoordinateFrame().inverse());

  }


  PLUGIN_API void VRG3DApp::onCursorMove(const VRCursorEvent &state)
  {

  }

  PLUGIN_API void VRG3DApp::onAnalogChange(const VRAnalogEvent &state)
  {

  }

  PLUGIN_API void VRG3DApp::onButtonDown(const VRButtonEvent &state)
  {

  }

  PLUGIN_API void VRG3DApp::onButtonUp(const VRButtonEvent &state)
  {

  }

  PLUGIN_API void VRG3DApp::onTrackerMove(const VRTrackerEvent &state)
  {

  }

}

