#include "VRMain.h"

#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "net/VRNetClient.h"
#include "net/VRNetServer.h"

namespace MinVR {


/** This helper class is a wrapper around a list of VRRenderHandlers that makes
    the collection of render handlers act like a single render handler.  VRMain
    holds a list of render handlers.  Technically, we could pass this list into
    the DisplayNode::render(..) method, so the composite render handler is not
    strictly necessary, but it simplifies the implmentation of display nodes a 
    bit to be able to write them as if they only need to handle a single 
    render handler object.
    TO DISCUSS: Should we expose this to the application programer?  It could be
    useful, but not sure we want to encourage using this outside of MinVR when
    we already have a way to register multiple renderhandlers.  Might be best to
    keep it simple.
*/
class VRCompositeRenderHandler : public VRRenderHandler {
public:
  VRCompositeRenderHandler(std::vector<VRRenderHandler*> &handlers) {
    _handlers = handlers;
  }
  virtual ~VRCompositeRenderHandler() {}

  virtual void onVRRenderScene(const VRDataIndex &renderState, VRData) {
    for (vector<VRRenderHandler*>::iterator it = _handlers.begin(); it != _handlers.end(); it++) {
      (*it)->onVRRenderScene(renderState);
    }
  }

  virtual void onVRRenderContext(const VRDataIndex &renderState) {
   for (vector<VRRenderHandler*>::iterator it = _handlers.begin(); it != _handlers.end(); it++) {
      (*it)->onVRRenderContext(renderState);
    }
  }
};



VRMain::VRMain() : _initialized(false), _config(NULL), _displayGraph(NULL), _factory(NULL), _index(NULL), _net(NULL), _pluginMgr(NULL)
{
  _factory = new VRFactory();
  _pluginMgr = new VRPluginMgr(this);
}


VRMain::~VRMain()
{
  if (_config) {
  	delete _config;
  }

  if (_displayGraph) {
  	delete _displayGraph;
  }

  if (_factory) {
  	delete _factory;
  }

  if (_index) {
  	delete _index;
  }

  if (_net) {
  	delete _net;
  }

  if (_pluginMgr) {
  	delete _pluginMgr;
  }
}


void 
VRMain::initialize(const std::string &configFile, const std::string &vrSetups) 
{
  _index = new VRDataIndex();
  _index->processXMLFile(configFile);


  // IDENTIFY THE VRSETUP(S) TO CONFIGURE

  if (!_index->exists("VRSetups","/")) {
    cerr << "VRMain Error:  No VRSetups tag found in the config file " << configFile << endl;
  	exit(1);
  }

  std::vector<std::string> vrSetupsInConfig = _index->getValue("VRSetups", "/");
  if (vrSetupsInConfig.size() == 1) {
    // only one VRSetup is defined in the config file, use it.
    _name = vrSetupList[0];
  }
  else {
    // more than one VRSetup is defined in this config file

	std::size_t found = vrSetups.find(",");
  	if (found == std::string::npos) {
  	  // no comma found in the vrSetups argument, so continue with just a single process and
  	  // configure based on the single vrSetup that was the user asked to start.
  	  _name = vrSetups;
  	}
  	else {
  	  // the vrSetups argument is a comma separated list of vrSetups, fork a new process for each
  	  // additional vrSetup.

  	  /**  TODO: with help from Dan O...
	
	  first, split the vrSetups string into an array vrSetupStrings

	  #ifndef WIN32
	    _name = vrSetupStrings[0]; // name of process 0
	    for (int i=1;i<vrSetupStrings.size();i++) {
	      pid_t pid = fork();
          if (pid == 0) {
            break;
          }
		  _name = vrSetupStrings[i];
        }
      #else
        // TODO: Add windows implementation of forking a process
      #endif

	  **/
  	}

  	// sanity check to make sure the vrSetup we are continuing with is actually defined in the config file
    if (std::find(vrSetupsInConfig.begin(), vrSetupsInConfig.end(), _name) == vrSetupsInConfig.end()) {
  	  cerr << "VRMain Error: The VRSetup " << _name << " was not found in the config file " << configFile << endl;
  	  exit(1);
  	}
  }

  // for everything from this point on, the VRSetup name for this process is stored in _name, and this
  // becomes the base namespace for all of the VRDataIndex lookups that we do.


  // LOAD PLUGINS:

  // Load plugins from the plugin directory.  This will add their factories to the master VRFactory.
  std::vector<std::string> pList = _index->getValue("VRPlugins", _name);
  for (std::vector<std::string>::iterator it = pList.begin(); it < pList.end(); ++it) {
  	
  	std::string path = _index->getValue("Path", _name + *it);
  	std::string file = _index->getValue("File", _name + *it);

    string buildType = "";
    #ifdef MinVR_DEBUG
      buildType = "d";
    #endif
  	
  	if (!_pluginMgr->loadPlugin(path, file + buildType)) {
  	  cerr << "VRMain Error: Problem loading plugin " << path << file << buildType << endl;
  	}
  }


  // CONFIGURE NETWORKING:

  // check the type of this VRSetup, it should be either "VRServer", "VRClient", or "VRStandAlone"
  std::string type = _index->getValue("Type", _name);
  if (type == "VRServer") {
  	std::string port = _index->getValue("Port", _name);
  	int numClients = _index->getValue("NumClients", _name);  	
  	_net = new VRNetServer(port, numClients);
  }
  else if (type == "VRClient") {
  	std::string ipAddress = _index->getValue("ServerIP", _name); 
  	std::string port = _index->getValue("ServerPort", _name);
  	_net = new VRNetClient(ipAddress, port);
  }
  else { // type == "VRStandAlone"
  	// no networking, leave _net=NULL
  }


  // CONFIGURE INPUT DEVICES:
  if (_index->exists("VRInputDevices", _name)) {
    std::vector<std::string> idList = _index->getValue("VRInputDevices", _name);
	  for (std::vector<std::string>::iterator it = idList.begin(); it < idList.end(); ++it) {
	    // create a new input device for each one in the list
	    VRInputDevice *dev = VRFactory::createInputDevice(this, _index, *it, _name);
	    if (dev) {
	  	  _inputDevices.push_back(dev);
	    }
	  }
  }


  // CONFIGURE GRAPHICS TOOLKITS
  if (_index->exists("VRGraphicsToolkits", _name)) {
    std::vector<std::string> idList = _index->getValue("VRGraphicsToolkits", _name);
    for (std::vector<std::string>::iterator it = idList.begin(); it < idList.end(); ++it) {
      // create a new graphics toolkit for each one in the list
      VRGraphicsToolkit *tk = VRFactory::createGraphicsToolkit(this, _index, *it, _name);
      if (tk) {
        _gfxToolkits.push_back(tk);
      }
    }
  }

  // CONFIGURE WINDOW TOOLKITS
  if (_index->exists("VRWindowToolkits", _name)) {
    std::vector<std::string> idList = _index->getValue("VRWindowToolkits", _name);
    for (std::vector<std::string>::iterator it = idList.begin(); it < idList.end(); ++it) {
      // create a new graphics toolkit for each one in the list
      VRWindowToolkit *tk = VRFactory::createWindowToolkit(this, _index, *it, _name);
      if (tk) {
        _winToolkits.push_back(tk);
      }
    }
  }

  // CONFIGURE THE DISPLAY GRAPH:
  if (_index->exists("VRDisplayGraph", _name)) {
    std::string dgstr = _index->getValue("VRDisplayGraph", _name);
    _displayGraph = VRFactory::createDisplayNode(this, _index, dgstr, _name);
  }

  _initialized = true;
}

void 
VRMain::synchronizeAndProcessEvents() 
{
  if (!initialized) {
    std::cerr << "VRMain not initialized." << std::endl;
    return;
  }
  
  VRDataQueue eventsFromDevices;
  for (int f = 0; f < _inputDevices.size(); f++) {
	_inputDevices[f]->appendNewInputEventsSinceLastCall(eventsFromDevices);
  }
  //eventsFromDevices.printQueue();

  // SYNCHRONIZATION POINT #1: When this function returns, we know
  // that all MinVR nodes have the same list of input events generated
  // since the last call to synchronizeAndProcessEvents(..).  So,
  // every node will process the same set of input events this frame.
  VRDataQueue::serialData eventData;
  if (_vrNet != NULL) {
    eventData = _vrNet->syncEventDataAcrossAllNodes(eventsFromDevices.serialize());
  }
  else if (eventsFromDevices.notEmpty()) {
  	// TODO: There is no need to serialize here if we are not a network node
	eventData = eventsFromDevices.serialize();
  }

  VRDataQueue *events = new VRDataQueue(eventData);
  while (events->notEmpty()) {
    // Unpack the next item from the queue.
    std::string event = _index->addSerializedValue( events->getSerializedObject() );

    // Invoke the user's callback on the new event
    for (int f = 0; f < _eventHandlers.size(); f++) {
      _eventHandlers[f]->onVREvent(event, _index);
    }

    // Get the next item from the queue.
    events->pop();
  }

  delete events;
}

void
VRMain::renderOnAllDisplays() 
{
  if (!initialized) {
    std::cerr << "VRMain not initialized." << std::endl;
    return;
  }

  VRDataIndex renderState;

  if (_displayGraph != NULL) {
    _displayGraph->render(renderState, VRCompositeRenderHandler(_renderHandlers));
 
    // TODO: Advanced: if you are really trying to optimize performance, this 
    // is where you might want to add an idle callback.  Here, it's
    // possible that the CPU is idle, but the GPU is still processing
    // graphics comamnds.

    _displayGraph->waitForRenderToComplete(renderState);
  }

  // SYNCHRONIZATION POINT #2: When this function returns we know that
  // all nodes have finished rendering on all their attached display
  // devices.  So, after this, we will be ready to "swap buffers",
  // simultaneously displaying these new renderings on all nodes.
  if (_net != NULL) {
    _net->syncSwapBuffersAcrossAllNodes();
  }

  if (_displayGraph != NULL) {
    _displayGraph->displayTheFinishedRendering(renderState);
  }
}


void 
VRMain::shutdown()
{
	// TODO
}


void 
VRMain::addEventHandler(VREventHandler* eventHandler) 
{
	_eventHandlers.push_back(eventHandler);
}

void 
VRMain::addRenderHandler(VRRenderHandler* renderHandler) 
{
	_renderHandlers.push_back(renderHandler);
}




/**
void VRMain::removeEventHandler(VREventHandler* eventHandler) {
	for (int f = 0; f < _eventHandlers.size(); f++)
	{
		if (_eventHandlers[f] == eventHandler)
		{
			_eventHandlers.erase(_eventHandlers.begin()+f);
			break;
		}
	}
}
**/

} // end namespace
