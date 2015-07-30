#ifndef VRGRAPHICSWINDOW_H
#define VRGRAPHICSWINDOW_H

#include "VRDisplayDevice.h"
#include "VRWindowSettings.h"
#include "VRViewport.h"
#include "VRWindowToolkit.h"

/**
*/
class VRGraphicsWindow : public VRDisplayDevice {
public:

  VRGraphicsWindow(VRWindowSettings settings, std::vector<VRViewport*> viewports, VRWindowToolkit windowToolkit) {
  	_windowHandle = _windowTookit->createWindow(settings);
  }

  virtual ~VRGraphicsWindow();

  virtual void handleUserInput(const std::vector<VREvent> &inputEvents);

  virtual int getNumRenderingPasses() { return _stereoFormatter->getNumRenderingPasses(); }

  virtual void startRenderingPass(int passNum, VRRenderState &state) {
  	_graphicsToolkit->setCurrentGraphicsContext(_windowHandle);
  	_graphicsToolkit->clear();
  	for (int pass=0; pass<getNumRenderingPasses(); pass++) {
  	  _stereoFormatter->startRenderingPass(passNum, state, _graphicsToolkit);
  	  _graphicsToolkit->setProjectionMatrix(_camera->getProjectionMatrix(state.projectionType));
  	}
  }

  virtual void endRenderingPass(int passNum, VRRenderState &state) {
    _stereoFormatter->endRenderingPass(passNum, state, _graphicsToolkit);
  }

  virtual void swapBuffers() {
  	_windowToolkit->swapBuffers();
  }


private:

  std::vector<VRViewport*> _viewports;

};

#endif

