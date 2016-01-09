#include "VRMain.h"

VRMain::VRMain() : initialized(false),_vrNet(NULL)
{
}


VRMain::~VRMain()
{
  delete _index;
  delete _vrNet;
}


void 
VRMain::initialize(const std::string settingsFile)
{
  initialize("", settingsFile);
}

void
VRMain::initialize(const std::string processName, const std::string settingsFile) 
{
  _name = processName;
  _index = new VRDataIndex();
  _index->processXMLFile(settingsFile, "/");

  _index->printStructure();
  
  // Load plugins from the plugin directory.  Once the available
  // plugins are identified, we can call their InputDeviceFactory and
  // DisplayDeviceFactory methods with the current settings.
  _pluginManager.addInterface(dynamic_cast<VRMain*>(this));
  if (_index->exists("MinVRDefaultPlugins", "/MVR/VRPlugins"))
  {
	  string buildType = "";
#ifdef MinVR_DEBUG
    buildType = "d";
#endif
	  std::vector<std::string> pluginNames = _index->getValue("Names", "/MVR/VRPlugins/MinVRDefaultPlugins");
	  for (int f = 0; f < pluginNames.size(); f++)
	  {
		 if (!_pluginManager.loadPlugin(std::string(PLUGINPATH) + "/" + pluginNames[f], pluginNames[f] + buildType))
		 {
			 _pluginManager.loadPlugin(std::string(PLUGININSTALLPATH) + "/" + pluginNames[f], pluginNames[f] + buildType);
		 }
	  }
  }

  // Create DisplayDevices from settings
  //std::vector<VRDisplayDevice*> displayDevices;
  //displayDevices.push_back(new ConsoleOnlyDisplay());

  // Create a Display Manager (either the default or multi-threaded)
  // based on settings
  //_displayManager = new DisplayManager(displayDevices);

  // Set Network Synchronization mode based on settings.  Note that
  // these names are hard-wired in place. This is meant to be not
  // configurable, though doubtless these names will change during
  // development.
  _vrNet = new VRNetClient((VRString)_index->getValue("/MVR/Server/Host"),
                           (VRString)_index->getValue("/MVR/Server/Port"));

  initialized = true;
}

void 
VRMain::synchronizeAndProcessEvents() {

  if (!initialized) {
    std::cerr << "VRMain not initialized." << std::endl;
    return;
  }
  
  VRDataQueue::serialData eventData;

  // std::vector<VREvent> inputEvents;
  // for (std::vector<InputDevice*>::iterator id = _inputDevices.begin();
  //     id != _inputDevices.end(); id++) {
  //  (*id)->appendNewInputEventsSinceLastCall(inputEvents);
  // }


  // SYNCHRONIZATION POINT #1: When this function returns, we know
  // that all MinVR nodes have the same list of input events generated
  // since the last call to synchronizeAndProcessEvents(..).  So,
  // every node will process the same set of input events this frame.
  if (_vrNet != NULL) {
    eventData = _vrNet->syncEventDataAcrossAllNodes(VRDataQueue::noData);
  }

  VRDataQueue *events = new VRDataQueue(eventData);

  // After MinVR's internal state is updated above, now the events are
  // passed on to the application programmer's event callback
  // function.
  while (events->notEmpty()) {

    // Step 2: Unpack the next item from the queue.
    std::string event =
      _index->addSerializedValue( events->getSerializedObject() );

    // The only internal responding to events done by MinVR is here.
    // This hook is typicaly used for displayDevices to update thier
    // states based upon the most recent head position from tracking
    // data.  It might also be useful for some other things, e.g., a 3D
    // audio display might also want to update its settings based on the
    // current position of the user.

    //  for (std::vector<DisplayDevice*>::iterator dd = _displayDevices.begin();
    //       dd != _displayDevices.end(); dd++) {  
    //    (*dd)->handleUserInput(event, _index);
    //  }

    // 2.5 Invoke the user's callback on the new event
    (*_eventCB)(event, _index);

    // Get the next item from the queue.
    events->pop();

  }

  delete events;
}

void 
VRMain::renderEverywhere() {

  if (!initialized) {
    std::cerr << "VRMain not initialized." << std::endl;
    return;
  }
  // for (std::vector<DisplayDevice*>::iterator dd = _displayDevices.begin();
  //      dd != _displayDevices.end(); dd++) {
  //   // Stereo displays will have two passes, one per eye; some
  //   // displays might require more
  //   for (int pass=0; pass < (*dd)->getNumRenderingPasses(); pass++) {

  //     // The display handles setting up the correct drawing buffer and
  //     // projection and view matrices
  //     (*dd)->startRenderingPass(pass, curState);

  //     // The application programmer fills in the
  //     // renderCallbackFunction to draw his/her scene
  (*_renderCB)(_index);

  //     (*dd)->endRenderingPass(pass, curState);
      
  //   }
  // }

  // SYNCHRONIZATION POINT #2: When this function returns we know that
  // all nodes have finished rendering on all their attached display
  // devices.  So, after this, we will be ready to "swap buffers",
  // simultaneously displaying these new renderings on all nodes.
  if (_vrNet != NULL) {
    _vrNet->syncSwapBuffersAcrossAllNodes();
  }

  (*_swapCB)();
}

// Adds the display factories for all plugins who use this interface
void VRMain::addVRDisplayDeviceFactory(MinVR::VRDisplayDeviceFactory* factory)
{
	_displayFactories.push_back(factory);
}

// Adds the input device factories for all plugins who use this interface
void VRMain::addVRInputDeviceFactory(VRInputDeviceFactory* factory)
{
	_inputDeviceFactories.push_back(factory);
}

// Used for timing (i.e. for animation, etc...)
void VRMain::addVRTimer(MinVR::VRTimer* timer) {
	_timers.push_back(timer);
}

void 
VRMain::shutdown()
{

}


