#ifndef VRVIEWPORT_H
#define VRVIEWPORT_H

#include "VRStereoFormatter.h"
#include "VRCameraPair.h"
#include "VREvent.h"
#include "VRRenderState.h"

/**
*/
class VRViewport {
public:

  VRViewport(VRRect geometry, VRStereoFormatter stereoFormatter, VRCameraPair cameraPair);
  virtual ~VRViewport();

  virtual void handleUserInput(const std::vector<VREvent> &inputEvents) {
  	_camera->handleUserInput(inputEvents);
  }

  virtual int getNumRenderingPasses() {
  	_stereoFormatter->getNumRenderingPasses();
  }

  virtual void startRenderingPass(int passNum, VRRenderState &state) {
  	_stereoFormatter->startRenderingPass(passNum, state);
  	_graphicsToolkit->setProjectionMatrix(_cameraPair->getProjectionMatrix(state.projectionType));
  }

  virtual void endRenderingPass(int passNum, VRRenderState &state) {
  	_stereoFormatter->endRenderingPass(passNum, state);
  }


private:

  VRRect _geometry;
  VRStereoFormatter _stereoFormatter;
  VRCameraPair _cameraPair;
};

#endif

