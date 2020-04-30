#include "VRG3DInputDevice.h"
#include <config/VRDataQueue.h>
#include "VRG3DWindowToolkit.h"
#include <GLG3D/RenderDevice.h>

namespace MinVR
{
  VRG3DInputDevice::VRG3DInputDevice()
  {

  }

  VRG3DInputDevice::~VRG3DInputDevice()
  {

  }

  void VRG3DInputDevice::appendNewInputEventsSinceLastCall(VRDataQueue* queue)
  {
    G3DWindow* gwindow = g3dRenderDevice->window();

    if (!gwindow)
    {
      return;
    }
    G3D::GEvent g3dEvent;

    gwindow->pollEvent(g3dEvent);
    
  }


  PLUGIN_API void VRG3DInputDevice::addWindow(G3DWindow* g3dWindow)
  {
    _windows.push_back(g3dWindow);
  }

}