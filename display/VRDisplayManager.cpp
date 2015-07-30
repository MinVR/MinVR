
#include "VRDisplayManager.h"


VRDisplayManager::VRDisplayManager(std::vector<DisplayDevice*> displayDevices) : _displayDevices(displayDevices) 
{
}

VRDisplayManager::~VRDisplayManager() 
{
}
  

void
VRDisplayManager::handleUserInput(const std::vector<VREvent> &inputEvents) {
  for (std::vector<DisplayDevice*>::iterator dd = _displayDevices.begin(); dd != _displayDevices.end(); dd++) {  
    // A head-tracked display must update itself each frame based upon the most recent head position from tracking data
    (*dd)->handleUserInput(inputEvents);
  }
}


void
VRDisplayManager::renderFrame(void (*renderCallback)(const VRRenderState &)) 
{
  VRRenderState curState;
  for (std::vector<DisplayDevice*>::iterator dd = _displayDevices.begin(); dd != _displayDevices.end(); dd++) {
	  // Stereo displays will have two passes, one per eye; some displays might require more
    for (int pass=0; pass < (*dd)->getNumRenderingPasses(); pass++) {

        // The display handles setting up the correct drawing buffer and projection and view matrices
        (*dd)->startRenderingPass(pass, curState);

        // All the app does in its draw method is push triangles down the pipe.  
        (*renderCallback)(curState);

        (*dd)->endRenderingPass(pass, curState);
      }
    }
  }


  virtual void swapBuffers() {
    for (std::vector<DisplayDevice*>::iterator dd = _displayDevices.begin(); dd != _displayDevices.end(); dd++) {
      (*dd)->swapBuffers();
    }
  }


