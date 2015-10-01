#include "VRMain.H"

VRMain::_instance = NULL;


VRMain::VRMain() : _vrNet(NULL)
{
}


VRMain::~VRMain()
{
  delete _instance;
  _instance = NULL;
}


VRMain* 
VRMain::instance() 
{
  if (!_instance) {
    _instance = new VRMain();
  }
  return _instance;
}


void 
VRMain::initialize(const VRSettings &settings) 
{
  // Load plugins from the plugin directory.  Once the available plugins are identified, we can call their
  // InputDeviceFactory and DisplayDeviceFactory methods with the current settings.


  // Create InputDevices from settings

  // Create DisplayDevices from settings
  std::vector<VRDisplayDevice*> displayDevices;
  displayDevices.push_back(new ConsoleOnlyDisplay());

  // Create a Display Manager (either the default or multi-threaded) based on settings
  _displayManager = new DisplayManager(displayDevices);

  // Set Network Synchronization mode based on settings

}

void 
VRMain::synchronizeAndProcessEvents(void (*eventCallbackFunction)(const std::vector<VREvent> &))
{
  std::vector<VREvent> inputEvents;
  for (std::vector<InputDevice*>::iterator id = _inputDevices.begin(); id != _inputDevices.end(); id++) {
    (*id)->appendNewInputEventsSinceLastCall(inputEvents);
  }

  // SYNCHRONIZATION POINT #1: When this function returns, we know that all MinVR nodes have the same list of
  // input events generated since the last call to synchronizeAndProcessEvents(..).  So, every node will process
  // the same set of input events this frame.
  if (_vrNet != NULL) {
    _vrNet->synchronizeInputEventsAcrossAllNodes(inputEvents);
  }

  // The only internal responding to events done by MinVR is here.  This hook is typicaly used for displayDevices to 
  // update thier states based upon the most recent head position from tracking data.  It might also be useful for
  // some other things, e.g., a 3D audio display might also want to update its settings based on the current position of
  // the user. 
  for (std::vector<DisplayDevice*>::iterator dd = _displayDevices.begin(); dd != _displayDevices.end(); dd++) {  
    (*dd)->handleUserInput(inputEvents);
  }

  // After MinVR's internal state is updated above, now the events are passed on to the application programmer's
  // event callback function.
  (*eventCallbackFunction)(inputEvents);
}

void 
VRMain::renderOnAllDisplayDevices(void (*renderCallbackFunction)(const VRRenderState &))
{
  VRRenderState curState;
  for (std::vector<DisplayDevice*>::iterator dd = _displayDevices.begin(); dd != _displayDevices.end(); dd++) {
    // Stereo displays will have two passes, one per eye; some displays might require more
    for (int pass=0; pass < (*dd)->getNumRenderingPasses(); pass++) {

      // The display handles setting up the correct drawing buffer and projection and view matrices
      (*dd)->startRenderingPass(pass, curState);

      // The application programmer fills in the renderCallbackFunction to draw his/her scene
      (*renderCallbackFunction)(curState);

      (*dd)->endRenderingPass(pass, curState);
      
    }
  }

  // SYNCHRONIZATION POINT #2: When this function returns we know that all nodes have finished rendering on all their
  // attached display devices.  So, after this, we will be ready to "swap buffers", simultaneously displaying these 
  // new renderings on all nodes.
  if (_vrNet != NULL) {
    _vrNet->synchronizeSwapBuffersAcrossAllNodes();
  }

  for (std::vector<DisplayDevice*>::iterator dd = _displayDevices.begin(); dd != _displayDevices.end(); dd++) {
    (*dd)->swapBuffers();
  }
}

void 
VRMain::shutdown()
{

}


