#include <main/VRMain.h>

#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#else
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
VRMain::initialize(int argc, char** argv)
{
  if ((argc < 2) || ((argc >= 2) && (std::string(argv[1]) == "-h"))) {
    std::cout << "MinVR Program Usage:" << std::endl;
    std::cout << std::string(argv[0]) + " <config-file-name.xml> [vrsetup-name]" << std::endl;
    std::cout << "     <config-file-name.xml> is required and is the name of a MinVR config file." << std::endl;
    std::cout << "     [vrsetup-name] is optional and is a comma-separated list of VRSetups" << std::endl;
    std::cout << "     to start.  If more than one VRSetup is listed, new processes will be forked." << std::endl;
    exit(0);
  }
  
  std::string configFile = argv[1];
  std::string vrSetupsToStart;
  if (argc >= 3) {
    vrSetupsToStart = argv[2];
  }
  
  
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
    std::list<std::string> names = _config->getValue("VRSetups","/MinVR/");
	for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
		if(_config->getDatum(*it)->hasAttribute("host")){	
			vrSetupsToStartArray.push_back(*it);
		}
	}
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

  // Fork a new process for each remaining vrsetup
#ifdef WIN32
  // Windows doesn't support forking, but it does allow us to create processes,
  // so we just create a new process with the config file to load as the first
  // command line argument and the vrsetup to start as the second command line
  // argument -- this means we need to enforce this convention for command line
  // arguments for all MinVR programs that want to support multiple processes.
  for (int i = 1; i < vrSetupsToStartArray.size(); i++) {
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682512(v=vs.85).aspx
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
	LPSTR title = new char[vrSetupsToStartArray[i].size() + 1];
	strcpy(title, vrSetupsToStartArray[i].c_str());
	si.lpTitle = title;
    
    std::string cmdLine = std::string(argv[0]) + " " + argv[1] + " " + vrSetupsToStartArray[i];
    
	LPSTR cmd = new char[cmdLine.size() + 1];
	strcpy(cmd, cmdLine.c_str());

    // Start the child process.
    if (!CreateProcess(NULL,   // No module name (use command line)
					   cmd,        // Command line
                       NULL,           // Process handle not inheritable
                       NULL,           // Thread handle not inheritable
                       FALSE,          // Set handle inheritance to FALSE
					   CREATE_NEW_CONSOLE,              // No creation flags
                       NULL,           // Use parent's environment block
                       NULL,           // Use parent's starting directory
                       &si,            // Pointer to STARTUPINFO structure
                       &pi )           // Pointer to PROCESS_INFORMATION structure
       ) {
      std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
      exit(1);
    }

	delete[] title;
	delete[] cmd;
  }
#else
  // On linux and OSX we can simply fork a new process for each vrsetup to start
  for (int i=1; i < vrSetupsToStartArray.size(); i++) {
    pid_t pid = fork();
    if (pid == 0) {
      break;
    }
    _name = vrSetupsToStartArray[i];
  }
#endif
  
  // sanity check to make sure the vrSetup we are continuing with is actually defined in the config file
  if (!_config->exists(_name)) {
    cerr << "VRMain Error: The VRSetup " << _name << " was not found in the config file " << configFile << endl;
    exit(1);
  }
  
  // for everything from this point on, the VRSetup name for this process is stored in _name, and this
  // becomes the base namespace for all of the VRDataIndex lookups that we do.


  // LOAD PLUGINS:

  // Load plugins from the plugin directory.  This will add their factories to the master VRFactory.
  if (_config->exists("VRPlugins", _name)) {
    
    std::list<std::string> names = _config->getValue("VRPlugins",_name);
    for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
		if(_config->getDatum(*it)->hasAttribute("pluginname") && _config->exists("Path",*it)){
			std::string path = _config->getValue("Path", *it);
			std::string file = _config->getDatum(*it)->getAttributeValue("pluginname");	
			
			string buildType = "";
#ifdef MinVR_DEBUG
			buildType = "d";
#endif
				
			if (!_pluginMgr->loadPlugin(path, file + buildType)) {
				cerr << "VRMain Error: Problem loading plugin " << path << file << buildType << endl;
			}
		}else if(_config->getDatum(*it)->hasAttribute("pluginlibfile")){
			std::string file = _config->getDatum(*it)->getAttributeValue("pluginlibfile");	
			if (!_pluginMgr->loadPlugin(file)) {
				cerr << "VRMain Error: Problem loading plugin " << file << endl;
			}
		}
    }
  }

  // CONFIGURE NETWORKING:

  // check the type of this VRSetup, it should be either "VRServer", "VRClient", or "VRStandAlone"
  if(_config->getDatum(_name)->hasAttribute("host")){		
	std::string type = _config->getDatum(_name)->getAttributeValue("host");
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
  }

  // CONFIGURE INPUT DEVICES:
  if (_config->exists("VRInputDevices", _name)) {
    std::string inputDevicesNameSpace = _config->getEntry("VRInputDevices", _name)->first;
    std::list<std::string> names = _config->getValue(inputDevicesNameSpace);
    inputDevicesNameSpace = inputDevicesNameSpace + '/';  
	for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
	  // create a new input device for each one in the list
	  std::string name_node = it->substr(inputDevicesNameSpace.size());      
	  VRInputDevice *dev = _factory->createInputDevice(this, _config, name_node, inputDevicesNameSpace);
	  if (dev) {
	   _inputDevices.push_back(dev);
	  }else if(_config->getDatum(name_node,inputDevicesNameSpace)->hasAttribute("inputdevice")){
        std::cerr << "Problem creating inputdevice: " << inputDevicesNameSpace << name_node << " with inputdevice="<< _config->getDatum(name_node,inputDevicesNameSpace)->getAttributeValue("inputdevice") << std::endl;
      }
	}
  }


  // CONFIGURE GRAPHICS TOOLKITS
  if (_config->exists("VRGraphicsToolkits", _name)) {
    std::string graphicsToolkitsNameSpace = _config->getEntry("VRGraphicsToolkits", _name)->first;
    std::list<std::string> names = _config->getValue(graphicsToolkitsNameSpace);
    graphicsToolkitsNameSpace = graphicsToolkitsNameSpace + '/';
    for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
      // create a new graphics toolkit for each one in the list
      std::string name_node = it->substr(graphicsToolkitsNameSpace.size());
      VRGraphicsToolkit *tk = _factory->createGraphicsToolkit(this, _config, name_node, graphicsToolkitsNameSpace);
      if (tk) {
        _gfxToolkits.push_back(tk);
      }
      else if(_config->getDatum(name_node,graphicsToolkitsNameSpace)->hasAttribute("graphicstoolkit")){
        std::cerr << "Problem creating graphics toolkit: " << graphicsToolkitsNameSpace << name_node << " with graphicstoolkit="<< _config->getDatum(name_node,graphicsToolkitsNameSpace)->getAttributeValue("graphicstoolkit") << std::endl;
      }
    }
  }

  // CONFIGURE WINDOW TOOLKITS
  if (_config->exists("VRWindowToolkits", _name)) {
    std::string windowwToolkitsNameSpace = _config->getEntry("VRWindowToolkits", _name)->first;
    std::list<std::string> names = _config->getValue(windowwToolkitsNameSpace);
    windowwToolkitsNameSpace = windowwToolkitsNameSpace + '/';
    for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
      // create a new graphics toolkit for each one in the list
      std::string name_node = it->substr(windowwToolkitsNameSpace.size());
      VRWindowToolkit *tk = _factory->createWindowToolkit(this, _config, name_node, windowwToolkitsNameSpace);
      if (tk) {
        _winToolkits.push_back(tk);
      }
      else if(_config->getDatum(name_node,windowwToolkitsNameSpace)->hasAttribute("windowtoolkit")){
        std::cerr << "Problem creating window toolkit: " << windowwToolkitsNameSpace << name_node << " with windowtoolkit="<< _config->getDatum(name_node,windowwToolkitsNameSpace)->getAttributeValue("windowtoolkit") << std::endl;
      }
    }
  }

  // CONFIGURE THE DISPLAY GRAPH:
  if (_config->exists("VRDisplayGraph", _name)) {
	std::string displayGraphNameSpace = _config->getEntry("VRDisplayGraph",_name)->first;
    std::list<std::string> names = _config->getValue(displayGraphNameSpace);
    displayGraphNameSpace = displayGraphNameSpace + '/';
	for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
		std::string name_node = it->substr(displayGraphNameSpace.size());
		_displayGraph = _factory->createDisplayNode(this, _config, name_node, displayGraphNameSpace);
	}
	if (_displayGraph == NULL) {
			std::cerr << "Problem creating display graph: " << displayGraphNameSpace << std::endl;	
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
