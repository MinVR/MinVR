#include "../include/VRG3DGraphicsToolkit.h"


namespace MinVR {
  

  PLUGIN_API VRG3DGraphicsToolkit::VRG3DGraphicsToolkit(VRMainInterface *vrMain)
    :_renderDevice(NULL), _vrMain(vrMain), isInitialized(false), _frameCounter(0)
  {
    _renderDevice = new G3D::RenderDevice();
  }



  PLUGIN_API VRG3DGraphicsToolkit::~VRG3DGraphicsToolkit()
  {
    
  }


  PLUGIN_API void VRG3DGraphicsToolkit::setDrawBuffer(VRDRAWBUFFER buffer)
  {
    /*_frameCounter++;
    if (_frameCounter == 1)
    {
      _renderDevice->beginFrame();
    }*/

    if (buffer == VRDRAWBUFFER_BACK) {
      _renderDevice->setDrawBuffer(G3D::RenderDevice::DRAW_BACK);
      glDrawBuffer(GL_BACK);
      _renderDevice->clear(true, true, true);
    }
    else if (buffer == VRDRAWBUFFER_FRONT) {
      _renderDevice->setDrawBuffer(G3D::RenderDevice::DRAW_FRONT);
    }
    else if (buffer == VRDRAWBUFFER_BACKLEFT) {
      _renderDevice->setDrawBuffer(G3D::RenderDevice::DRAW_BACK_LEFT);
    }
    else if (buffer == VRDRAWBUFFER_FRONTLEFT) {
      _renderDevice->setDrawBuffer(G3D::RenderDevice::DRAW_FRONT_LEFT);
    }
    else if (buffer == VRDRAWBUFFER_BACKRIGHT) {
      _renderDevice->setDrawBuffer(G3D::RenderDevice::DRAW_BACK_RIGHT);
    }
    else if (buffer == VRDRAWBUFFER_FRONTRIGHT) {
      _renderDevice->setDrawBuffer(G3D::RenderDevice::DRAW_FRONT_RIGHT);
    }
  }
  

  PLUGIN_API void VRG3DGraphicsToolkit::flushGraphics()
  {

  }

  PLUGIN_API void VRG3DGraphicsToolkit::finishGraphics()
  {

  }

  PLUGIN_API VRGraphicsToolkit* VRG3DGraphicsToolkit::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace)
  {
    return new VRG3DGraphicsToolkit(vrMain);
  }

  void VRG3DGraphicsToolkit::InitG3DRenderDevice( G3D::OSWindow* g3dWindow)
  {
    if (!isInitialized)
    {
      std::cout << "G3D Render device init" << std::endl;
      _renderDevice->init(g3dWindow);
      std::cout << "Render device reset state" << std::endl;
     // _renderDevice->resetState();

      isInitialized = true;
    }
    

  }


  G3D::RenderDevice* VRG3DGraphicsToolkit::getG3DRenderDevice()
  {
    return this->_renderDevice;
  }

}