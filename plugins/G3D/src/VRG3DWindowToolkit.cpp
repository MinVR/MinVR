#include "VRG3DWindowToolkit.h"
#include <GLG3D/OSWindow.h>

#ifdef G3D_WIN32
#    include <GLG3D/Win32Window.h>
#elif defined(G3D_OSX)
#    include <GLG3D/CocoaWindow.h>
#else
#    include <GLG3D/SDLWindow.h>
#endif

#include "VRG3DInputDevice.h"

namespace MinVR {



  VRG3DWindowToolkit::VRG3DWindowToolkit(VRMainInterface * vrMain)
    : _vrMain(vrMain), _inputDev(NULL)
  {
    _inputDev = new VRG3DInputDevice();
  }

  VRG3DWindowToolkit::~VRG3DWindowToolkit()
  {
  }

  PLUGIN_API int VRG3DWindowToolkit::createWindow(VRWindowSettings settings)
  {
    G3DWindow* g3dWindow = createG3DWindow(settings);

    if (_windows.size() == 0) {
      // if this is the first window created, then register the virtual input device
      // with VRMain so that VRMain will start polling GLFW for input events
      _vrMain->addInputDevice(_inputDev);
    }
    _inputDev->addWindow(g3dWindow);


    if (g3dWindow)
    {
      _windows.push_back(g3dWindow);
    }

    return (int)_windows.size() - 1;
  }

  PLUGIN_API void VRG3DWindowToolkit::makeWindowCurrent(int windowID)
  {
    _windows[windowID]->makeCurrent();
  }

  PLUGIN_API void VRG3DWindowToolkit::destroyWindow(int windowID)
  {

  }

  PLUGIN_API void VRG3DWindowToolkit::swapBuffers(int windowID)
  {
    _windows[windowID]->swapGLBuffers();
  }

  PLUGIN_API VRWindowToolkit * VRG3DWindowToolkit::create(VRMainInterface * vrMain, VRDataIndex * config, const std::string & nameSpace)
  {

    return new VRG3DWindowToolkit(vrMain);
  }

  PLUGIN_API void VRG3DWindowToolkit::getFramebufferSize(int windowID, int& width, int& height)
  {
    G3DWindow::Settings settings;
    G3DWindow* g3dWindow = _windows[windowID];
    width = g3dWindow->width();
    height = g3dWindow->height();
  }

  G3DWindow * VRG3DWindowToolkit::createG3DWindow(VRWindowSettings settings)
  {
    G3DWindow* g3dWindow =  NULL;

#   ifdef G3D_WIN32
      g3dWindow = G3D::Win32Window::create();
#   elif defined(G3D_OSX)
      g3dWindow = G3D::CocoaWindow::create();
#   else
      g3dWindow = G3D::SDLWindow::create();
#   endif
      
   
    //debugAssert(g3dWindow == NULL);
    return g3dWindow;

  }

}