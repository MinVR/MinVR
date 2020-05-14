#include "../include/VRG3DDisplayNode.h"
#include "../include/VRG3DGraphicsToolkit.h"
#include "../include/VRG3DWindowToolkit.h"

namespace MinVR {
  PLUGIN_API VRG3DDisplayNode::
    VRG3DDisplayNode(const std::string &name, VRGraphicsToolkit *gfxToolkit, VRWindowToolkit *winToolkit, const VRWindowSettings &settings, int windowId)
    :VRDisplayNode(name), _windowID(windowId), _gfxToolkit(gfxToolkit),_winToolkit(winToolkit),
    _settings(settings)
  {
    _valuesAdded.push_back("WindowX");
    _valuesAdded.push_back("WindowY");
    _valuesAdded.push_back("WindowWidth");
    _valuesAdded.push_back("WindowHeight");
    _valuesAdded.push_back("FramebufferWidth");
    _valuesAdded.push_back("FramebufferHeight");
    _valuesAdded.push_back("SharedContextId");
    _valuesAdded.push_back("WindowID");
  }

  PLUGIN_API VRG3DDisplayNode::~VRG3DDisplayNode()
  {

  }


  PLUGIN_API void VRG3DDisplayNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler)
  {
    renderState->pushState();

    // Is this the kind of state information we expect to pass from one node to the next?
    renderState->addData("IsGraphics", 1);
    renderState->addData("WindowX", _settings.xpos);
    renderState->addData("WindowY", _settings.ypos);
    renderState->addData("WindowWidth", _settings.width);
    renderState->addData("WindowHeight", _settings.height);
    renderState->addData("FramebufferWidth", _framebufferWidth);
    renderState->addData("FramebufferHeight", _framebufferHeight);
    renderState->addData("SharedContextId", _settings.sharedContextGroupID);
    renderState->addData("WindowID", _windowID);

    
    //renderState->addData("G3DRenderDevice", dynamic_cast<VRG3DGraphicsToolkit*>(_gfxToolkit)->getG3DRenderDevice());

    _winToolkit->makeWindowCurrent(_windowID);
    /*if (_settings.quadBuffered)
    {
      _gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACKRIGHT);
      //_gfxToolkit->clearScreen();
      _gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACKLEFT);
      //_gfxToolkit->clearScreen();
    }
    else
    {
      _gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACK);
      //_gfxToolkit->clearScreen();
    }*/

    // windows should call the application programmer's context-level callback
    renderHandler->onVRRenderContext(*renderState);

    VRDisplayNode::render(renderState, renderHandler);

    _gfxToolkit->flushGraphics();

    renderState->popState();
  }

  PLUGIN_API VRDisplayNode* VRG3DDisplayNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace)
  {
    

    VRWindowSettings settings;
    settings.xpos = config->getValueWithDefault("XPos", 100, nameSpace);
    settings.ypos = config->getValueWithDefault("YPos", 100, nameSpace);
    settings.width = config->getValueWithDefault("Width", 640, nameSpace);
    settings.height = config->getValueWithDefault("Height", 640, nameSpace);
    settings.border = (int)config->getValueWithDefault("Border", 0, nameSpace);
    settings.visible = (int)config->getValueWithDefault("Visible", 0, nameSpace);
    settings.caption = std::string(config->getValueWithDefault("Caption", std::string("MinVR Window"), nameSpace));
    settings.quadBuffered = (int)config->getValueWithDefault("QuadBuffered", 0, nameSpace);
    settings.sharedContextGroupID = int(config->getValueWithDefault("SharedContextGroupID", -1, nameSpace));
    settings.contextVersionMajor = int(config->getValueWithDefault("ContextVersionMajor", 1, nameSpace));
    settings.contextVersionMinor = int(config->getValueWithDefault("ContextVersionMinor", 2, nameSpace));
    settings.rgbBits = int(config->getValueWithDefault("RGBBits", 8, nameSpace));
    settings.alphaBits = int(config->getValueWithDefault("AlphaBits", 8, nameSpace));
    settings.depthBits = int(config->getValueWithDefault("DepthBits", 24, nameSpace));
    settings.stencilBits = int(config->getValueWithDefault("StencilBits", 8, nameSpace));
    settings.fullScreen = int(config->getValueWithDefault("FullScreen", 0, nameSpace));
    settings.resizable = int(config->getValueWithDefault("Resizable", 1, nameSpace));
    settings.allowMaximize = int(config->getValueWithDefault("AllowMaximize", 1, nameSpace));
    settings.gpuAffinity = int(config->getValueWithDefault("UseGPUAffinity", 1, nameSpace));
    settings.debugContext = int(config->getValueWithDefault("UseDebugContext", 0, nameSpace));
    settings.msaaSamples = int(config->getValueWithDefault("MSAASamples", 0, nameSpace));

    //  std::cout << "Window corners: " << settings.xpos << ", " << settings.ypos << ", " << settings.width << ", " << settings.height << std::endl;

    std::string wtk = config->getValue("WindowToolkit", nameSpace);
    VRWindowToolkit *winToolkit = vrMain->getWindowToolkit(wtk);
    if (winToolkit == NULL) {
      std::cerr << "Cannot get the window toolkit named: " << wtk << std::endl;
    }

    std::string nodeNameSpace = nameSpace;

    std::string gtk = config->getValue("GraphicsToolkit", nodeNameSpace);
    VRGraphicsToolkit *gfxToolkit = vrMain->getGraphicsToolkit(gtk);
    if (gfxToolkit == NULL) {
      std::cerr << "Cannot get the graphics toolkit named: " << gtk << std::endl;
    }

    int newWindowId = winToolkit->createWindow(settings);

    G3D::OSWindow* g3dWindow = 
      dynamic_cast<VRG3DWindowToolkit*>(winToolkit)->getG3DWindow(newWindowId);
    
    dynamic_cast<VRG3DGraphicsToolkit*>(gfxToolkit)->
      InitG3DRenderDevice(g3dWindow);

    dynamic_cast<VRG3DWindowToolkit*>(winToolkit)->setG3DRenderDevice(
      dynamic_cast<VRG3DGraphicsToolkit*>(gfxToolkit)->getG3DRenderDevice());

    VRDisplayNode *node = new VRG3DDisplayNode(nameSpace, gfxToolkit, winToolkit, settings, newWindowId);

    return node;
  }

  PLUGIN_API G3D::RenderDevice* VRG3DDisplayNode::getG3DRenderDevice()
  {
    VRG3DGraphicsToolkit* g3dGraphicsTK = dynamic_cast<VRG3DGraphicsToolkit*>(_gfxToolkit);
    if (g3dGraphicsTK)
    {
      return g3dGraphicsTK->getG3DRenderDevice();
    }
    return nullptr;
  }


  PLUGIN_API void VRG3DDisplayNode::waitForRenderToComplete(VRDataIndex *renderState)
  {
   // VRDisplayNode::waitForRenderToComplete(renderState);
   // _winToolkit->makeWindowCurrent(_windowID);
   // _gfxToolkit->finishGraphics();
  }

  PLUGIN_API void VRG3DDisplayNode::displayFinishedRendering(VRDataIndex *renderState)
  {
    VRDisplayNode::displayFinishedRendering(renderState);
    _winToolkit->makeWindowCurrent(_windowID);
    _winToolkit->swapBuffers(_windowID);
  }

}


