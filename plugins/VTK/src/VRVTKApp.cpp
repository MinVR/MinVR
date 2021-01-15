


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
    
      ren = vtkSmartPointer<vtkRenderer>::New();
      vtkNew<vtkExternalOpenGLRenderWindow> renWin;
      externalVTKWidget->SetRenderWindow(renWin.GetPointer());
      ren->SetActiveCamera(camera);
      renWin->AddRenderer(ren);
   
    }
   
  }

  void VRVTKApp::onRenderGraphicsScene(const VRGraphicsState& state)
  {
    /*
      Set up Projection and Camera-World Matrix
    */
    externalVTKWidget->GetRenderWindow()->Render();
   
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

