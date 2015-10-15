#ifndef VRGRAPHICSWINDOW_H
#define VRGRAPHICSWINDOW_H

#include "VRDisplayDevice.h"
#include "VRWindowSettings.h"
#include "VRViewport.h"
#include "VRWindowToolkit.h"


class VRWindowSettings {
public:
  bool hasBorder;
  std::string title;
  int GPUAffinityID;
  // etc..
};


/**
*/
class VRGraphicsWindow : public VRDisplayDevice {
public:

  VRGraphicsWindow(VRWindowSettings settings, std::vector<VRViewport*> viewports, VRAbstractGraphcisToolkit graphicsToolkit) {
    _viewports = viewports;
    _graphicsToolkit = graphicsToolkit;
  	_windowHandle = _graphicsTookit->createWindow(settings);
  }

  virtual ~VRGraphicsWindow();

  virtual void handleUserInput(const std::vector<VREvent> &inputEvents);

  virtual int getNumRenderingPasses() { 
    int max = 1;
    for (std::vector<VRViewport*>::iterator vp=_viewports.begin(); vp < _viewports.end(); vp++) {
      if (vp->getNumRenderingPasses() > max) {
        max = vp->getNumRenderingPasses();
      }
    }
    return max;
  }

  virtual void startRenderingPass(int passNum, VRRenderState &state) {
  	_graphicsToolkit->setCurrentGraphicsContext(_windowHandle);
  	_graphicsToolkit->clear();
    for (std::vector<VRViewport*>::iterator vp=_viewports.begin(); vp < _viewports.end(); vp++) {
      vp->startRenderingPass(passNum, state);
    }
  }

  virtual void endRenderingPass(int passNum, VRRenderState &state) {
    _graphicsToolkit->setCurrentGraphicsContext(_windowHandle);
    for (std::vector<VRViewport*>::iterator vp=_viewports.begin(); vp < _viewports.end(); vp++) {
      vp->endRenderingPass(passNum, state);
    }
  }

  virtual void swapBuffers() {
    _graphicsToolkit->setCurrentGraphicsContext(_windowHandle);
    _graphicsToolkit->swapBuffers();
  }


private:

  std::vector<VRViewport*> _viewports;

};

#endif

