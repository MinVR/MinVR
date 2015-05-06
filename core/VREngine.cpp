#include "VREngine.h"



VREngine*
VREngine::create(const std::string &pluginDir, const std::map<std::string,std::string> &configMap, GraphicsEngine *gfxEngine)
{
  VREngine *vrEngine = new VREngine(gfxEngine);
  bool success = true;

  // Load plugins, registering their factory methods with this VREngine
  

  // Create DisplayDevices and InputDevices based upon settings defined in configMap

  vrEngine->addDisplayDevice(new DummyDisplayDevice());

  if (success) {
    return vrEngine;
  }
  else {
    return NULL;
  }
}


VREngine::VREngine(GraphicsEngine *gfxEngine) : _gfxEngine(gfxEngine)
{
}

VREngine::~VREngine()
{
}


void
VREngine::run(VRApp *app)
{
  while (loopOnce(app)) {
  }
  shutdown();
}


bool
VREngine::loopOnce(VRApp *app)
{
  std::vector<VREvent*> inputEvents;
  for (std::vector<InputDevice*>::iterator id = _inputDevices.begin(); id != _inputDevices.end(); id++) {
    (*id)->pollForInput(inputEvents);
  }

  if (_vrNet != NULL) {
    _vrNet->synchronizeInputEventsAcrossAllNodes(inputEvents);
  }

  app->handleUserInput(inputEvents);
  app->updateStateBeforeDrawing();

  // TODO Threads: For multi-threaded rendering, this for loop would be replaced with one thread per DisplayDevice
  for (std::vector<DisplayDevice*>::iterator dd = _displayDevices.begin(); dd != _displayDevices.end(); dd++) {
    // A head-tracked display must update itself each frame based upon the most recent head position from tracking data
    (*dd)->handleUserInput(inputEvents);

    // Stereo displays will have two passes, one per eye; some displays might support more, e.g., 2D textual graphics 
    // overlayed on top of the 3D renderings
    // or a third pass that somehow blends together the results of the first two
    for (int pass=0; pass < (*dd)->getNumRenderingPasses(); pass++) {

      // The display handles setting up the correct drawing buffer and projection and view matrices
      (*dd)->startRenderingPass(pass);

      // All the app does in its draw method is push triangles down the pipe.  
      app->drawGraphics();

      // The display must call swapbuffers at the end of the final rendering pass
      (*dd)->endRenderingPass(pass);

    }


    if (_vrNet != NULL) {
      _vrNet->synchronizeSwapBuffersAcrossAllNodes();
    }


    // TODO: Not sure if swapBuffers should be a property of the gfxEngine or the DisplayDevice... check what MinVR does with threads
    _gfxEngine->swapBuffers();
  }

  return true;
}


void
VREngine::shutdown()
{
}




