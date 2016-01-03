#include "VRMain.H"

VRMain* VRMain::_instance = NULL;


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
VRMain::initialize(const std::string settingsFile) 
{
  _index = new VRDataIndex();
  _index->processXMLFile(settingsFile, "/");

  _index->printStructure();
  
  // Load plugins from the plugin directory.  Once the available
  // plugins are identified, we can call their InputDeviceFactory and
  // DisplayDeviceFactory methods with the current settings.


  // Create DisplayDevices from settings
  //std::vector<VRDisplayDevice*> displayDevices;
  //displayDevices.push_back(new ConsoleOnlyDisplay());

  // Create a Display Manager (either the default or multi-threaded)
  // based on settings
  //_displayManager = new DisplayManager(displayDevices);

  // Set Network Synchronization mode based on settings

}

void 
VRMain::synchronizeAndProcessEvents(void (*eventCB)
                                   (const std::string &,
                                    VRDataIndex *)) {

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

  //  ATTENTION: The user callback should use just ONE event, not a
  //  whole list of them.  It should be integrated into the MinVR
  //  processing of events.  That is, the events should be dequeued
  //  and processed, one at a time, and *once*.  Probably this means
  //  that the above event handlers also must be rewritten to process
  //  just one event at a time, and this function will contain the
  //  unwrapping, dequeueing, and processing of events, with the
  //  display callback and the user callback interleaved.

  
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
    (*eventCB)(event, _index);

    // Get the next item from the queue.
    events->pop();

  }
}

void 
VRMain::renderOnAllDisplayDevices(void (*renderCallbackFunction)(VRDataIndex *))
{

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
  //     (*renderCallbackFunction)(curState);

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

  // for (std::vector<DisplayDevice*>::iterator dd = _displayDevices.begin();
  //      dd != _displayDevices.end(); dd++) {
  //   (*dd)->swapBuffers();
  // }
}

void 
VRMain::shutdown()
{

}


