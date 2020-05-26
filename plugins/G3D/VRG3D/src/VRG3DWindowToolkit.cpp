#include "../include/VRG3DWindowToolkit.h"
#include <GLG3D/OSWindow.h>

#ifdef G3D_WIN32
#    include <GLG3D/Win32Window.h>
#elif defined(G3D_OSX)
#    include <GLG3D/CocoaWindow.h>
#else
#    include <GLG3D/SDLWindow.h>
#endif

#include "../include/VRG3DInputDevice.h"
#include "../include/VRG3DGraphicsToolkit.h"

namespace MinVR {



  VRG3DWindowToolkit::VRG3DWindowToolkit(VRMainInterface * vrMain)
    : _vrMain(vrMain), _inputDev(NULL), _frameCounter(0)
  {
    _inputDev = new VRG3DInputDevice();
  }

  VRG3DWindowToolkit::~VRG3DWindowToolkit()
  {
  }

  PLUGIN_API int VRG3DWindowToolkit::createG3DWindow(const G3DWindow::Settings& settings)
  {
    G3DWindow* g3dWindow = NULL;

#   ifdef G3D_WIN32
    g3dWindow = G3D::Win32Window::create(settings);
    //g3dWindow = G3D::Win32Window::create();
    
#   elif defined(G3D_OSX)
    g3dWindow = G3D::CocoaWindow::create();
#   else
    g3dWindow = G3D::SDLWindow::create();
#   endif

       
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
    _frameCounter++;
    if (_frameCounter == 1)
    {
      g3dRenderDevice->beginFrame();
    }
  }

  PLUGIN_API void VRG3DWindowToolkit::destroyWindow(int windowID)
  {

  }

  PLUGIN_API void VRG3DWindowToolkit::swapBuffers(int windowID)
  {
    g3dRenderDevice->endFrame();
    g3dRenderDevice->beginFrame();
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


  PLUGIN_API int VRG3DWindowToolkit::createWindow(VRWindowSettings settings)
  {
    G3DWindow::Settings g3dSettings;
    g3dSettings.width = settings.width;
    g3dSettings.height = settings.height;
    g3dSettings.framed = settings.border;
    g3dSettings.x = settings.xpos;
    g3dSettings.y = settings.ypos;
    g3dSettings.rgbBits = settings.rgbBits;
    g3dSettings.alphaBits = settings.alphaBits;
    g3dSettings.depthBits = settings.depthBits;
    g3dSettings.stencilBits = settings.stencilBits;
    g3dSettings.msaaSamples = settings.msaaSamples;
    g3dSettings.fullScreen = settings.fullScreen;
    g3dSettings.resizable = settings.resizable;
    g3dSettings.allowMaximize = settings.allowMaximize;
    g3dSettings.msaaSamples = settings.msaaSamples;
    g3dSettings.caption = settings.caption;


    int winId = createG3DWindow(g3dSettings);

    return winId;
  }

  PLUGIN_API G3D::OSWindow* VRG3DWindowToolkit::getG3DWindow(int windId)
  {
    return _windows[windId];
  }

  PLUGIN_API void VRG3DWindowToolkit::setG3DRenderDevice(G3D::RenderDevice* rd)
  {
    g3dRenderDevice = rd;
    this->_inputDev->g3dRenderDevice = g3dRenderDevice;
  }

  PLUGIN_API G3D::RenderDevice* VRG3DWindowToolkit::getG3DRenderDevice()
  {
    return g3dRenderDevice;
  }

  

}