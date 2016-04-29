#include <main/VRMain.h>

#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include <display/VRConsoleNode.h>
#include <display/VRGraphicsWindowNode.h>
#include <display/VRGroupNode.h>
#include <display/VROffAxisProjectionNode.h>
#include <display/VRStereoNode.h>
#include <display/VRViewportNode.h>
#include <net/VRNetClient.h>
#include <net/VRNetServer.h>
#include <plugin/VRPluginManager.h>

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

  virtual void onVRRenderScene(VRDataIndex *renderState, VRDisplayNode *callingNode) {
    for (std::vector<VRRenderHandler*>::iterator it = _handlers.begin(); it != _handlers.end(); it++) {
      (*it)->onVRRenderScene(renderState, callingNode);
    }
  }

  virtual void onVRRenderContext(VRDataIndex *renderState, VRDisplayNode *callingNode) {
    for (std::vector<VRRenderHandler*>::iterator it = _handlers.begin(); it != _handlers.end(); it++) {
      (*it)->onVRRenderContext(renderState, callingNode);
    }
  }
  
protected:
  std::vector<VRRenderHandler*> _handlers;
};



  
VRMain::VRMain() : _initialized(false), _config(NULL), _net(NULL), _factory(NULL), _pluginMgr(NULL), _displayGraph(NULL)
{
  _factory = new VRFactory();
  
  // add sub-factories that are part of the MinVR core library right away
  _factory->addSubFactory(new VRConsoleNodeFactory());
  _factory->addSubFactory(new VRGraphicsWindowNodeFactory());
  _factory->addSubFactory(new VRGroupNodeFactory());
  _factory->addSubFactory(new VROffAxisProjectionNodeFactory());
  _factory->addSubFactory(new VRStereoNodeFactory());
  _factory->addSubFactory(new VRViewportNodeFactory());
  
  _pluginMgr = new VRPluginManager(this);
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

  if (_config) {
  	delete _config;
  }

  if (_net) {
  	delete _net;
  }

  if (_pluginMgr) {
  	delete _pluginMgr;
  }
}


void 
VRMain::initialize(const std::string &configFile, const std::string &vrSetupsToStart)
{

  _config = new VRDataIndex();
  if (!_config->processXMLFile(configFile,"/")) {
  }

  // IDENTIFY THE VRSETUP(S) TO CONFIGURE

  if (!_config->exists("VRSetups","/MinVR/")) {
    cerr << "VRMain Error:  No VRSetups tag found in the config file " << configFile << endl;
    exit(1);
  }
  
  VRStringArray vrSetupsToStartArray;
  if (vrSetupsToStart == "") {
    // no vrSetupsToStart are specified, start all of VRSetups listed in the config file
    vrSetupsToStartArray = _config->getValue("VRSetups", "/MinVR/");
  }
  else {
    // a comma-separated list of vrSetupsToStart was provided
    VRString elem;
    std::stringstream ss(vrSetupsToStart);
    while (std::getline(ss, elem, ',')) {
      vrSetupsToStartArray.push_back(elem);
    }
  }
  
  // This process will be the first one listed
  _name = vrSetupsToStartArray[0];

  // Fork a new process for each remaining process
#ifdef WIN32
  // Windows doesn't have forking, but we are so early in the execution at this
  // point, it should work fine to use the Windows CreateProcess() function to
  // startup the same exe with the cmd line arguments:  configFile vrSetupsToStartArray[i]
  for (int i = 1; i < vrSetupsToStartArray.size(); i++) {
	  std::cerr << "Forking processes not yet implemented on windows." << std::endl;
  }
#else
  for (int i=1; i < vrSetupsToStartArray.size(); i++) {
    pid_t pid = fork();
    if (pid == 0) {
      break;
    }
    _name = vrSetupsToStartArray[i];
  }
#endif

  // sanity check to make sure the vrSetup we are continuing with is actually defined in the config file
  std::vector<std::string> vrSetupsInConfig = _config->getValue("VRSetups", "/MinVR/");
  if (std::find(vrSetupsInConfig.begin(), vrSetupsInConfig.end(), _name) == vrSetupsInConfig.end()) {
    cerr << "VRMain Error: The VRSetup " << _name << " was not found in the config file " << configFile << endl;
    exit(1);
  }

  _name = "/MinVR/" + _name + "/";
  
  
  // for everything from this point on, the VRSetup name for this process is stored in _name, and this
  // becomes the base namespace for all of the VRDataIndex lookups that we do.


  // LOAD PLUGINS:

  // Load plugins from the plugin directory.  This will add their factories to the master VRFactory.
  if (_config->exists("VRPlugins", _name)) {
    std::vector<std::string> pList = _config->getValue("VRPlugins", _name);
    for (std::vector<std::string>::iterator it = pList.begin(); it < pList.end(); ++it) {
      
      std::string path = _config->getValue("Path", "/MinVR/" + *it);
      std::string file = _config->getValue("File", "/MinVR/" + *it);
      
      string buildType = "";
#ifdef MinVR_DEBUG
      buildType = "d";
#endif
      
      if (!_pluginMgr->loadPlugin(path, file + buildType)) {
        cerr << "VRMain Error: Problem loading plugin " << path << file << buildType << endl;
      }
    }
  }

  // CONFIGURE NETWORKING:

  // check the type of this VRSetup, it should be either "VRServer", "VRClient", or "VRStandAlone"
  std::string type = _config->getValue("Type", _name);
  if (type == "VRServer") {
  	std::string port = _config->getValue("Port", _name);
  	int numClients = _config->getValue("NumClients", _name);  	
  	_net = new VRNetServer(port, numClients);
  }
  else if (type == "VRClient") {
    std::string port = _config->getValue("Port", _name);
  	std::string ipAddress = _config->getValue("ServerIP", _name); 
  	_net = new VRNetClient(ipAddress, port);
  }
  else { // type == "VRStandAlone"
  	// no networking, leave _net=NULL
  }


  // CONFIGURE INPUT DEVICES:
  if (_config->exists("VRInputDevices", _name)) {
    std::vector<std::string> idList = _config->getValue("VRInputDevices", _name);
	  for (std::vector<std::string>::iterator it = idList.begin(); it < idList.end(); ++it) {
	    // create a new input device for each one in the list
	    VRInputDevice *dev = _factory->createInputDevice(this, _config, *it, "/MinVR/");
	    if (dev) {
	  	  _inputDevices.push_back(dev);
	    }
	  }
  }


  // CONFIGURE GRAPHICS TOOLKITS
  if (_config->exists("VRGraphicsToolkits", _name)) {
    std::vector<std::string> idList = _config->getValue("VRGraphicsToolkits", _name);
    for (std::vector<std::string>::iterator it = idList.begin(); it < idList.end(); ++it) {
      // create a new graphics toolkit for each one in the list
      VRGraphicsToolkit *tk = _factory->createGraphicsToolkit(this, _config, *it, "/MinVR/");
      if (tk) {
        _gfxToolkits.push_back(tk);
      }
      else {
        std::cerr << "Problem creating graphics toolkit: " << *it << std::endl;
      }
    }
  }

  // CONFIGURE WINDOW TOOLKITS
  if (_config->exists("VRWindowToolkits", _name)) {
    std::vector<std::string> idList = _config->getValue("VRWindowToolkits", _name);
    for (std::vector<std::string>::iterator it = idList.begin(); it < idList.end(); ++it) {
      // create a new graphics toolkit for each one in the list
      VRWindowToolkit *tk = _factory->createWindowToolkit(this, _config, *it, "/MinVR/");
      if (tk) {
        _winToolkits.push_back(tk);
      }
      else {
        std::cerr << "Problem creating window toolkit: " << *it << std::endl;
      }
    }
  }

  // CONFIGURE THE DISPLAY GRAPH:
  if (_config->exists("VRDisplayGraph", _name)) {
    std::string dgstr = _config->getValue("VRDisplayGraph", _name);
    _displayGraph = _factory->createDisplayNode(this, _config, dgstr, "/MinVR/");
    if (_displayGraph == NULL) {
      std::cerr << "Problem creating display graph: " << dgstr << std::endl;
    }
  }

  _initialized = true;
}

void 
VRMain::synchronizeAndProcessEvents() 
{
  if (!_initialized) {
    std::cerr << "VRMain not initialized." << std::endl;
    return;
  }
  
  VRDataQueue eventsFromDevices;
  for (int f = 0; f < _inputDevices.size(); f++) {
	_inputDevices[f]->appendNewInputEventsSinceLastCall(&eventsFromDevices);
  }
  //eventsFromDevices.printQueue();

  // SYNCHRONIZATION POINT #1: When this function returns, we know
  // that all MinVR nodes have the same list of input events generated
  // since the last call to synchronizeAndProcessEvents(..).  So,
  // every node will process the same set of input events this frame.
  VRDataQueue::serialData eventData;
  if (_net != NULL) {
    eventData = _net->syncEventDataAcrossAllNodes(eventsFromDevices.serialize());
  }
  else if (eventsFromDevices.notEmpty()) {
  	// TODO: There is no need to serialize here if we are not a network node
	eventData = eventsFromDevices.serialize();
  }

  VRDataQueue *events = new VRDataQueue(eventData);
  while (events->notEmpty()) {
    // Unpack the next item from the queue.
    std::string event = _config->addSerializedValue( events->getSerializedObject() );

    // Invoke the user's callback on the new event
    for (int f = 0; f < _eventHandlers.size(); f++) {
      _eventHandlers[f]->onVREvent(event, _config);
    }

    // Get the next item from the queue.
    events->pop();
  }

  delete events;
}

void
VRMain::renderOnAllDisplays() 
{
  if (!_initialized) {
    std::cerr << "VRMain not initialized." << std::endl;
    return;
  }

  VRDataIndex renderState;

  if (_displayGraph != NULL) {
    VRCompositeRenderHandler compositeHandler(_renderHandlers);
    _displayGraph->render(&renderState, &compositeHandler);
 
    // TODO: Advanced: if you are really trying to optimize performance, this 
    // is where you might want to add an idle callback.  Here, it's
    // possible that the CPU is idle, but the GPU is still processing
    // graphics comamnds.

    _displayGraph->waitForRenderToComplete(&renderState);
  }

  // SYNCHRONIZATION POINT #2: When this function returns we know that
  // all nodes have finished rendering on all their attached display
  // devices.  So, after this, we will be ready to "swap buffers",
  // simultaneously displaying these new renderings on all nodes.
  if (_net != NULL) {
    _net->syncSwapBuffersAcrossAllNodes();
  }

  if (_displayGraph != NULL) {
    _displayGraph->displayFinishedRendering(&renderState);
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


VRGraphicsToolkit* 
VRMain::getGraphicsToolkit(const std::string &name) {
  for (std::vector<VRGraphicsToolkit*>::iterator it = _gfxToolkits.begin(); it < _gfxToolkits.end(); ++it) {
    //std::cout << (*it)->getName() << std::endl;
    if ((*it)->getName() == name) {
      return *it;
    }
  }
  
  return NULL;
}

VRWindowToolkit* 
VRMain::getWindowToolkit(const std::string &name) {
  for (std::vector<VRWindowToolkit*>::iterator it = _winToolkits.begin(); it < _winToolkits.end(); ++it) {
    //std::cout << (*it)->getName() << std::endl;
    if ((*it)->getName() == name) {
      return *it;
    }
  }
  return NULL;
}

void 
VRMain::addInputDevice(VRInputDevice* dev) {
  _inputDevices.push_back(dev);
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
