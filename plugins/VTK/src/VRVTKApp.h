

#ifndef VRG3DVRAPP_H
#define VRG3DVRAPP_H

#include <api/MinVR.h>
#include <ExternalVTKWidget.h>
#include <vtkExternalOpenGLCamera.h>

namespace MinVR
{

  class VRVTKApp : public VRApp
  {

  public:
    PLUGIN_API VRVTKApp(int argc, char *argv[]);
    PLUGIN_API virtual ~VRVTKApp();
    PLUGIN_API virtual void onRenderGraphicsContext(const VRGraphicsState& state);
    PLUGIN_API virtual void onRenderGraphicsScene(const VRGraphicsState& state);
    PLUGIN_API virtual void onCursorMove(const VRCursorEvent &state);
    PLUGIN_API virtual void onAnalogChange(const VRAnalogEvent &state);
    PLUGIN_API virtual void onButtonDown(const VRButtonEvent &state);
    PLUGIN_API virtual void onButtonUp(const VRButtonEvent &state);
    PLUGIN_API virtual void onTrackerMove(const VRTrackerEvent &state);


  protected:
    vtkNew<ExternalVTKWidget> externalVTKWidget;
    vtkSmartPointer<vtkRenderer> ren;
    vtkSmartPointer<vtkExternalOpenGLCamera> camera;
    vtkNew<vtkExternalOpenGLRenderWindow> renWin;
  };
}




#endif // !VRG3DVRAPP




