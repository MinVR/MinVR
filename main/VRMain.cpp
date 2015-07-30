#include "VRCore.H"

VRCore::VRCore() : _instance(NULL) 
{
}


VRCore::~VRCore() 
{
  delete _instance;
}


VRCore* 
VRCore::instance() 
{
  if (!_instance) {
    _instance = new VRCore();
  }
  return _instance;
}


void 
VRCore::initialize(const VRSettings &settings) 
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
VRCore::synchronizeAndProcessEvents(void (*eventCallbackFunction)(const std::vector<VREvent> &))
{
  std::vector<VREvent> inputEvents;
  for (std::vector<InputDevice*>::iterator id = _inputDevices.begin(); id != _inputDevices.end(); id++) {
    (*id)->appendNewInputEventsSinceLastCall(inputEvents);
  }

  if (_vrNet != NULL) {
    _vrNet->synchronizeInputEventsAcrossAllNodes(inputEvents);
  }

  _displayManager->handleUserInput(inputEvents);

  (*eventCallbackFunction)(inputEvents);
}

void 
VRCore::renderOnAllDisplayDevices(void (*renderCallbackFunction)(const VRRenderState &))
{
  _displayManager->renderFrame(renderCallbackFunction);

  if (_vrNet != NULL) {
    _vrNet->synchronizeSwapBuffersAcrossAllNodes();
  }

  _displayManager->swapBuffers();
}

void 
VRCore::shutdown()
{

}


