


#include <api/VRApp.h>
#include "VRVTKApp.h"

namespace MinVR
{
  VRVTKApp::VRVTKApp(int argc, char *argv[]) :
    VRApp(argc, argv)
  {    

   
  }

  VRVTKApp::~VRVTKApp()
  {

  }

  void VRVTKApp::onRenderGraphicsContext(const VRGraphicsState& state)
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

  void VRVTKApp::onRenderGraphicsScene(const VRGraphicsState& state)
  {
    /*
      Set up Projection and Camera-World Matrix
    */
    
   
  }


  PLUGIN_API void VRVTKApp::onCursorMove(const VRCursorEvent &state)
  {

  }

  PLUGIN_API void VRVTKApp::onAnalogChange(const VRAnalogEvent &state)
  {

  }

  PLUGIN_API void VRVTKApp::onButtonDown(const VRButtonEvent &state)
  {

  }

  PLUGIN_API void VRVTKApp::onButtonUp(const VRButtonEvent &state)
  {

  }

  PLUGIN_API void VRVTKApp::onTrackerMove(const VRTrackerEvent &state)
  {

  }

}

