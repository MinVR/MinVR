#include <main/VRMain.h>

#include <main/VRSystem.h>

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
#include <display/VRProjectionNode.h>
#include <display/VRLookAtNode.h>
#include <display/VRTrackedLookAtNode.h>
#include <input/VRFakeTrackerDevice.h>
#include <net/VRNetClient.h>
#include <net/VRNetServer.h>
#include <plugin/VRPluginManager.h>
#include <input/VRFakeTrackerDeviceRelative.h>
#include <sstream>
#include <main/VREventInternal.h>
#include <cstdlib>

// TESTARG is true is the strings match and are not empty.
#define TESTARG(ARG, CMDSTR) ((!CMDSTR.empty()) && (ARG.compare(0, CMDSTR.size(), CMDSTR) == 0))

// This checks an argument to an option (i.e. "-s CMDSTR") to see if
// it's of the form '-s=XXX' (all in CMDSTR) or '-s XXX' (need
// argv[i+1]), then adds the argument to CMDLIST.
#define EXECARG(CMDSTR, CMDLIST)       \
  if (arg.size() > CMDSTR.size()) {       \
    CMDLIST.push_back(arg.substr(CMDSTR.size() + 1));    \
  } else if (argc > i+1) { \
    std::string argString = std::string(argv[++i]); \
    if (!recursing) _originalCommandLine += argString + " "; \
    CMDLIST.push_back(argString); \
  } else { \
    VRERROR("Something is wrong with the " + CMDSTR + " option.", \
            "It needs an argument."); \
  }


namespace MinVR {

bool VRParseCommandLine::parseCommandLine(int argc, char** argv,
                                          bool recursing) {

  // Our goal here is simply to accumulate all the config data that
  // needs to be collected, and add it all at once after the whole
  // command line has been parsed.  There may be multiple key=val
  // arguments and multiple config files, too.  We also look for -h
  // and -N options, but we leave the rest to the application
  // programmer, via the _leftoverArgc and _leftoverArgv params.

  std::vector<std::string> configValList;
  std::vector<std::string> configFileList;

  _execute = true;

  if (!recursing) {
    _originalCommandLine = "";
    _leftoverCommandLine = "";
    _leftoverArgc = 0;
  }

  for (int i = 0; i < argc; i++) {

    std::string arg = std::string(argv[i]);
    if (!recursing) _originalCommandLine += arg + " ";

    if (arg[0] != '-') {
      // Not something we can use.  Add it to the leftovers.
      _leftoverCommandLine += arg + " ";

      _leftoverArgv[_leftoverArgc] = (char*)malloc(arg.size() + 1);
      strcpy(_leftoverArgv[_leftoverArgc++], argv[i]);

    } else if (TESTARG(arg, _setConfigValueShort)) {

      EXECARG(_setConfigValueShort, configValList);

    } else if (TESTARG(arg, _setConfigValueLong)) {

      EXECARG(_setConfigValueLong, configValList);

    } else if (TESTARG(arg, _loadConfigShort)) {

      EXECARG(_loadConfigShort, configFileList);

    } else if (TESTARG(arg, _loadConfigLong)) {

      EXECARG(_loadConfigLong, configFileList);

    } else if (TESTARG(arg, _minVRData)) {

      // We have an encoded command-line here, and it requires an = sign.
      if (arg.size() > _minVRData.size()) {
        decodeMinVRData(arg.substr(_minVRData.size() + 1));

      } else {
        VRERROR("Something is wrong with the MINVR_DATA= option.",
                "It needs an argument.");
      }

      // There is nothing else of interest on the command line.
      break;

    } else if (TESTARG(arg, _helpShort)) {

      VRSearchConfig vsc;
      help(vsc);

    } else if (TESTARG(arg, _helpLong)) {

      VRSearchConfig vsc;
      help(vsc);

    } else if (TESTARG(arg, _noExecute)) {

      _execute = false;

    } else {
      // We do not want this argument, add it to the leftovers.
      _leftoverCommandLine += arg + " ";

      _leftoverArgv[_leftoverArgc] = (char*)malloc(arg.size());
      strcpy(_leftoverArgv[_leftoverArgc++], argv[i]);
    }
  }

  // Now we have a complete list of the config files and key=val
  // pairs.  Add them.

  if (!configFileList.empty()) {
    for (std::vector<std::string>::iterator it = configFileList.begin();
         it != configFileList.end(); it++) {

      loadConfig(*it);
    }
  }

  if (!configValList.empty()) {
    for (std::vector<std::string>::iterator it = configValList.begin();
         it != configValList.end(); it++) {

      setConfigValue(*it);
    }
  }

  return _execute;
}

std::string VRParseCommandLine::argsToData(const std::string &argStr) {
    std::string encodedData =
      base64_encode((unsigned char const*)argStr.c_str(), argStr.size());
    return _minVRData + "=" + encodedData;
}

std::string VRParseCommandLine::dataToArgs(const std::string &payload) {

  return base64_decode(payload);
}


void VRParseCommandLine::decodeMinVRData(const std::string &payload) {

  //std::cout << "DECODE LINE: " << payload << std::endl;

  std::string decodedCommandLine = dataToArgs(payload);

  // Break it up into argc and argv.
  std::stringstream argStream(decodedCommandLine);
  int newArgc = 0;
  char* newArgv[60];

  while (argStream) {
    std::string arg;
    argStream >> arg;

    // Ignore zero-length args.
    if (arg.size() < 1) continue;

    //std::cout << "processing decoded >" << arg << "<" << std::endl;

    newArgv[newArgc] = new char[arg.size()];
    strcpy(newArgv[newArgc++], arg.c_str());
  }

  // Call parseCommandLine() recursively.  Keep accumulating leftovers.
  parseCommandLine(newArgc, newArgv, _leftoverArgc);

  // reclaim memory of newArgv
  for (int i = newArgc - 1; i >= 0; i--) {
    delete newArgv[i];
  }
}


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



std::string getCurrentWorkingDir()
{
#ifdef WIN32
	//not tested
	char cwd[1024];
	GetCurrentDirectory( 1024, cwd);
	return std::string(cwd);
#else
	char cwd[1024];
	char * tmp = getcwd(cwd, sizeof(cwd));
	return std::string(tmp);
#endif

}


VRMain::VRMain() : _initialized(false), _config(NULL), _net(NULL), _factory(NULL), _pluginMgr(NULL), _frame(0), _shutdown(false)
{
  _config = new VRDataIndex();
	_factory = new VRFactory();
	// add sub-factories that are part of the MinVR core library right away
	_factory->registerItemType<VRDisplayNode, VRConsoleNode>("VRConsoleNode");
	_factory->registerItemType<VRDisplayNode, VRGraphicsWindowNode>("VRGraphicsWindowNode");
	_factory->registerItemType<VRDisplayNode, VRGroupNode>("VRGroupNode");
	_factory->registerItemType<VRDisplayNode, VROffAxisProjectionNode>("VROffAxisProjectionNode");
    _factory->registerItemType<VRDisplayNode, VRProjectionNode>("VRProjectionNode");
	_factory->registerItemType<VRDisplayNode, VRStereoNode>("VRStereoNode");
	_factory->registerItemType<VRDisplayNode, VRViewportNode>("VRViewportNode");
	_factory->registerItemType<VRDisplayNode, VRLookAtNode>("VRLookAtNode");
	_factory->registerItemType<VRDisplayNode, VRTrackedLookAtNode>("VRTrackedLookAtNode");
  _factory->registerItemType<VRInputDevice, VRFakeTrackerDevice>("VRFakeTrackerDevice");
      _factory->registerItemType<VRInputDevice, VRFakeTrackerDeviceRelative>("VRFakeTrackerDeviceRelative");
    _pluginMgr = new VRPluginManager(this);
}


VRMain::~VRMain()
{

	if (_config) {
		delete _config;
	}

	if (!_inputDevices.empty()) {
		for (std::vector<VRInputDevice*>::iterator it = _inputDevices.begin(); it != _inputDevices.end(); ++it) delete *it;
	}

	if (!_displayGraphs.empty()) {
		for (std::vector<VRDisplayNode*>::iterator it = _displayGraphs.begin(); it != _displayGraphs.end(); ++it) delete *it;
	}

	if (!_gfxToolkits.empty()) {
		for (std::vector<VRGraphicsToolkit*>::iterator it = _gfxToolkits.begin(); it != _gfxToolkits.end(); ++it) delete *it;
	}

	if (!_winToolkits.empty()) {
		for (std::vector<VRWindowToolkit*>::iterator it = _winToolkits.begin(); it != _winToolkits.end(); ++it) delete *it;
	}

	if (_factory) {
		delete _factory;
	}

	if (_net) {
		delete _net;
	}

	if (_pluginMgr) {
		delete _pluginMgr;
	}
}


void VRMain::loadConfig(const std::string &configName) {

  std::string fileName = _configPath.findFile(configName);
  if (fileName.empty()) {

    VRERROR("Cannot find a configuration named " + configName + ".",
            "Checked: " + _configPath.getPath());
  } else {

    DEBUGMSG("Loading configuration: " + fileName);

    bool success = _config->processXMLFile(fileName, "/");

    if (!success) {
      VRERROR("Could not process XML file " + fileName,
              "Something may be wrong with the file.");
    }
  }
}

void VRMain::setConfigValue(const std::string &keyAndValStr) {
  std::string name = _config->addData(keyAndValStr);
}

void VRMain::_startSSHProcess(const std::string &setupName,
                              const bool noSSH) {

  // First, get the machine where it is to be started.
  std::string nodeIP = _config->getValue("HostIP", setupName);

  // Now get the path where it has to be started and create a
  // command to get us there.
  std::string workingDirectory = getCurrentWorkingDir();
  std::string command = "cd " + workingDirectory + ";";

  // If we have to adjust the display, set that in the command.
  if (_config->exists("HostDisplay", setupName)) {
    std::string displayVar = _config->getValue("HostDisplay",setupName);
    command += " export DISPLAY=" + displayVar + " ;";
  }

  // These arguments are to be added to the process
  std::string processSpecificArgs =
    " " + getSetConfigValueLong() + " VRSetupsToStart=" + setupName +
    " " + getSetConfigValueLong() + " StartedSSH=1";

  std::string logFile = "";
  if (_config->exists("LogToFile", setupName)) {
    logFile = " >" + (VRString)_config->getValue("LogToFile",setupName) + " 2>&1 ";
  } else {
    logFile = " >/dev/null 2>&1 ";
  }

  std::string sshcmd;
  sshcmd = "ssh " + nodeIP +
    " '" + command + getOriginalCommandLine() + processSpecificArgs + logFile +
    " &' &";

  // Start the client, at least if the noSSH flag tells us to.
  SHOWMSG("Starting " + sshcmd);

  if (noSSH) {
    SHOWMSG("(Not starting:" + sshcmd + ")");
  } else {
    system(sshcmd.c_str());
  }
}

bool VRMain::_startLocalProcess(const std::string &setupName) {
  // Fork a new process for each remaining vrsetup to be run locally.
  // Unfortunately, this is OS-specific.

#ifdef WIN32
  // Windows doesn't support forking, but it does allow us to create
  // processes, so we just create a new process.

  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682512(v=vs.85).aspx
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));
  LPSTR title = new char[setupName + 1];
  strcpy(title, (setupName).c_str());
  si.lpTitle = title;

  std::string processSpecificArgs =
    " " + getSetConfigValueLong() + " VRSetupsToStart=" + setupName;
  std::string cmdLine = getOriginalCommandLine() + processSpecificArgs;

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
                     &pi )           // Ptr to PROCESS_INFORMATION structure
      ) {
    VRERRORNOADV("CreateProcess failed: " + GetLastError());
  }

  delete[] title;
  delete[] cmd;

  return false;
#else

  // On linux and OSX we can simply fork a new process for each
  // vrsetup to start

  pid_t pid = fork();
  if (pid == 0) {
    _name = setupName;
    return true; // This is the child.
  } else {
    return false; // This is the parent.
  }

#endif
}


void VRMain::initialize(int argc, char **argv) {

  bool execute = parseCommandLine(argc, argv);

  if (_config->empty()) {

    VRERROR("No config data available.",
            "Something is wrong with your configuration file specification.");
  }

  // Create an empty list of VRSetups that need starting.
	VRStringArray vrSetupsToStartArray;

	if (_config->exists("VRSetupsToStart","/")) {

    // A comma-separated list of vrSetupsToStart was provided.  Copy
    // it into our list.
		std::string vrSetupsToStart = _config->getValue("VRSetupsToStart","/");
		VRString elem;
		std::stringstream ss(vrSetupsToStart);
		while (std::getline(ss, elem, ',')) {
			vrSetupsToStartArray.push_back(elem);
		}

	} else {

    // No vrSetupsToStart are explicitly listed.  So start all of
		// VRSetups listed in the config file.
    if (_config->exists("/MinVR/VRSetups")) {

      VRContainer names = _config->getValue("/MinVR/VRSetups");
      for (VRContainer::const_iterator it = names.begin();
           it != names.end(); ++it) {

        vrSetupsToStartArray.push_back("/MinVR/VRSetups/" + *it);
      }
    } else {

      // We have no list of setups.  Let's try starting everything with a
      // 'hostType' attribute.
      VRContainer names = _config->selectByAttribute("hostType","*");
      for (VRContainer::const_iterator it = names.begin();
           it != names.end(); ++it) {

        vrSetupsToStartArray.push_back(*it);
      }
    }
  }

  // Check to see if we've got anything to start.
	if (vrSetupsToStartArray.empty()) {

    VRERROR("No VRSetups to start are defined.",
            "Your configuration must contain at least one VRSetup element.");

	}

  // STEP 1: Loop through the setups to start.  If they belong on another
  // machine, ssh them over there and let them run.  Adopt the first one
  // that starts on this machine.  If there are more than one to be started
  // on this machine, fork (or the Win equivalent) them into separate
  // processes.

  for (vector<std::string>::iterator it = vrSetupsToStartArray.begin();
       it != vrSetupsToStartArray.end(); it++)  {

    if (_config->exists("HostIP", *it) && !_config->exists("StartedSSH", "/")) {

      // Remote setups need to be started via ssh.
      _startSSHProcess(*it);

    } else {

      if (_name.empty()) {

        // The first local process might as well be this one.
        _name = *it;

      } else {

        // This method returns true for a spawned child process.
        if (_startLocalProcess(*it)) break;

      }
    }
  }

  // All the processes with names have been started.  If this process
  // has no name, it must not be necessary to keep it going.
  if (_name.empty()) {
    SHOWMSG("All VRSetups have been started - Exiting.");
    exit(1);
  } else {
    SHOWMSG("Starting node " + _name);
  }

  // STEP 4:  Sanity check to make sure the vrSetup we are continuing with is
  // actually defined in the config settings that have been loaded.
	if (!_config->exists(_name)) {
    VRERROR("VRMain Error: The VRSetup " +
            _name + " has not been loaded through a config file.",
            "Your config file must contain a VRSetup element.");
  }

  // For everything from this point on, the VRSetup name for this process is
  // stored in _name, and this becomes the base namespace for all of the
  // VRDataIndex lookups that we do.


    // STEP 5: LOAD PLUGINS:

	// Load plugins from the plugin directory.  This will add their
	// factories to the master VRFactory.


  // MinVR will try to load plugins based on a search path.  To specify
  // custom paths for an application, use VRSearchPlugin.addPathEntry(), or
  // add it to the configuration through a configuration file, or the
  // command line.
  //
  // Any search path specified in the config file is prepended to the
  // default search path.
  if (_config->exists("/MinVR/PluginPath")) {
    _pluginSearchPath.digestPathString(_config->getValue("/MinVR/PluginPath"));
  }

  // Get the objects for which a pluginType is specified.
  std::list<std::string> names = _config->selectByAttribute("pluginType", "*");

  // Sort through the objects returned.
  for (std::list<std::string>::const_iterator it = names.begin();
       it != names.end(); it++) {

    // Get the name of the plugin specified for each object.
    std::string pluginName = _config->getAttributeValue(*it, "pluginType");

    // Find the actual library that is the plugin, and load it, if possible.
    std::string fullLibName = _pluginSearchPath.findFile(pluginName);
    if(!_pluginMgr->loadPlugin(fullLibName)) {
      VRWARNING("VRMain Error: Problem loading plugin: " + pluginName,
                "Could not load from any of the following paths: " +
                _pluginSearchPath.getPath());
    }
  }


	// STEP 6: CONFIGURE NETWORKING:

	// Check the type of this VRSetup, it should be either "VRServer",
	// "VRClient", or "VRStandAlone"
  if(_config->hasAttribute(_name, "hostType")){
    std::string type = _config->getAttributeValue(_name, "hostType");
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

	// STEP 7: CONFIGURE INPUT DEVICES:
	{
		std::list<std::string> names = _config->selectByAttribute("inputdeviceType", "*", _name);
		for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
			// create a new input device for each one in the list
			VRInputDevice *dev = _factory->create<VRInputDevice>(this, _config, *it);
			if (dev) {
				_inputDevices.push_back(dev);
			}
			else{
				std::cerr << "Problem creating inputdevice: " << *it << " with inputdeviceType=" << _config->getAttributeValue(*it, "inputdeviceType") << std::endl;
			}
		}
	}



  // If the program was run with --no-execute, we can stop here.
  if (!execute) exit(1);

  ///////////////  Ok, now execute.



	// STEP 8: CONFIGURE WINDOWS
	{

    // Find all the display nodes.
    std::list<std::string> displayNodeNames =
      _config->selectByAttribute("displaynodeType", "*", _name, true);

    // Loop through the display nodes, creating graphics toolkits where necessary.
		for (std::list<std::string>::const_iterator it = displayNodeNames.begin();
         it != displayNodeNames.end(); ++it) {
			// CONFIGURE GRAPHICS TOOLKIT

      std::string graphicsToolkitName =
        _config->getByAttribute("graphicstoolkitType", "*", *it);

      // If there is no graphics toolkit to be found here, we can't
      // really do anything, so throw an error.
      if (graphicsToolkitName.empty()) {
        VRWARNING("No graphics toolkit found in:" + *it,
                  "Is there an element with 'graphicstoolkitType' specified in your config file?");
      }

      // Create a new graphics toolkit.
      VRGraphicsToolkit *gtk =
        _factory->create<VRGraphicsToolkit>(this, _config, graphicsToolkitName);

      if (gtk) {
        _config->addData(*it + "/GraphicsToolkit", gtk->getName());

        bool exists = false;
        // Check to see if this toolkit is already in our list.
        for (std::vector<VRGraphicsToolkit*>::iterator it_gfxToolkits =
               _gfxToolkits.begin();
             it_gfxToolkits != _gfxToolkits.end(); ++it_gfxToolkits) {

          if ((*it_gfxToolkits)->getName() == gtk->getName()) {
            // If it already exists, then never mind.
            exists = true;
            delete gtk;
            break;
          }
        }
        // If this toolkit isn't already in the list, add it.
        if (!exists) _gfxToolkits.push_back(gtk);

      } else {

        VRERROR("Problem creating graphics toolkit: " +
                graphicsToolkitName +
                " with graphicstoolkit=" +
                _config->getAttributeValue(graphicsToolkitName,
                                           "graphicstoolkitType"),
                "The create failed, so there might be a link error to a plugin, or a misconfiguration of the graphics toolkit.");
      }


      // CONFIGURE WINDOW TOOLKIT

      std::string windowToolkitName =
        _config->getByAttribute("windowtoolkitType", "*", *it);

      // If there is no window toolkit to be found here, we can't
      // really do anything, so throw an error.
      if (windowToolkitName.empty()) {
        VRERROR("No window toolkit found in:" + *it,
                "Is there an element with 'windowtoolkitType' in your config file?");
      }

      // Create a new window toolkit.
      VRWindowToolkit *wtk =
        _factory->create<VRWindowToolkit>(this, _config, windowToolkitName);

      if (wtk) {
        _config->addData(*it + "/WindowToolkit", wtk->getName());

        bool exists = false;
        for (std::vector<VRWindowToolkit*>::iterator it_winToolkits =
               _winToolkits.begin();
             it_winToolkits != _winToolkits.end(); ++it_winToolkits) {

          if ((*it_winToolkits)->getName() == wtk->getName()) {
            exists = true;
            delete wtk;
            break;
          }
        }
        if (!exists) _winToolkits.push_back(wtk);

      }	else {
        VRERROR("Problem creating window toolkit: " +
                windowToolkitName +
                " with windowtoolkitType=" +
                _config->getAttributeValue(windowToolkitName,
                                           "windowtoolkitType"),
                "The create failed, so there might be a link error to a plugin, or a misconfiguration of the window toolkit.");
      }

			// add window to the displayGraph list
			VRDisplayNode *dg = _factory->create<VRDisplayNode>(this, _config, *it);
			if (dg) {
				_displayGraphs.push_back(dg);
			}
			else{
        VRWARNINGNOADV("Problem creating window: " + *it + " with windowType=" +
                       _config->getAttributeValue(*it, "windowType"));
			}
		}
	}


  // std::cerr << "DISPLAY NODES:" << std::endl;
  // for (std::vector<VRDisplayNode*>::iterator it = _displayGraphs.begin();
  //      it != _displayGraphs.end(); it++) {
  //   std::cerr << *(*it) << std::endl;
  // }

	_initialized = true;
    _shutdown = false;
}

void
VRMain::synchronizeAndProcessEvents()
{
	if (!_initialized) {
		throw std::runtime_error("VRMain not initialized.");
	}

	VRDataQueue eventsFromDevices;
    std::string event = "FrameStart";
    std::string dataField = "/ElapsedSeconds";
    _config->addData(event + dataField, (float)VRSystem::getTime());
    eventsFromDevices.push(_config->serialize(event));

	for (size_t f = 0; f < _inputDevices.size(); f++) {
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
		std::string eventName = _config->addSerializedValue( events->getSerializedObject() );

        VREventInternal event(eventName, _config);

		// Invoke the user's callback on the new event
		for (size_t f = 0; f < _eventHandlers.size(); f++) {
			_eventHandlers[f]->onVREvent(*event.getAPIEvent());
		}

		// Get the next item from the queue.
		events->pop();
	}

	delete events;
}

void
VRMain::renderOnAllDisplays()
{
  if (!_initialized) throw std::runtime_error("VRMain not initialized.");

	VRDataIndex renderState;
	renderState.addData("/InitRender", _frame == 0);

	if (!_displayGraphs.empty()) {
		VRCompositeRenderHandler compositeHandler(_renderHandlers);
		for (std::vector<VRDisplayNode*>::iterator it = _displayGraphs.begin(); it != _displayGraphs.end(); ++it) (*it)->render(&renderState, &compositeHandler);

		// TODO: Advanced: if you are really trying to optimize performance, this
		// is where you might want to add an idle callback.  Here, it's
		// possible that the CPU is idle, but the GPU is still processing
		// graphics comamnds.

		for (std::vector<VRDisplayNode*>::iterator it = _displayGraphs.begin(); it != _displayGraphs.end(); ++it) (*it)->waitForRenderToComplete(&renderState);
	}

	// SYNCHRONIZATION POINT #2: When this function returns we know that
	// all nodes have finished rendering on all their attached display
	// devices.  So, after this, we will be ready to "swap buffers",
	// simultaneously displaying these new renderings on all nodes.
	if (_net != NULL) {
		_net->syncSwapBuffersAcrossAllNodes();
	}

	if (!_displayGraphs.empty()) {
		for (std::vector<VRDisplayNode*>::iterator it = _displayGraphs.begin(); it != _displayGraphs.end(); ++it) (*it)->displayFinishedRendering(&renderState);
	}

	_frame++;
}

std::list<std::string>
VRMain::auditValuesFromAllDisplays()
{
  std::list<std::string> out;

  if (!_initialized) throw std::runtime_error("VRMain not initialized.");

	if (!_displayGraphs.empty()) {

		for (std::vector<VRDisplayNode*>::iterator it = _displayGraphs.begin();
         it != _displayGraphs.end(); ++it) {
      std::map<std::string,std::string> subOut = (*it)->getValuesAdded();

      for (std::map<std::string,std::string>::iterator jt = subOut.begin();
           jt != subOut.end(); jt++) {
        out.push_back(jt->first + " : " + jt->second);
      }
    }
  }

  return out;
}


void
VRMain::shutdown()
{
    _shutdown = true;
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
		if ((*it)->getName() == name) {
			return *it;
		}
	}

	return NULL;
}

VRWindowToolkit*
VRMain::getWindowToolkit(const std::string &name) {
	for (std::vector<VRWindowToolkit*>::iterator it = _winToolkits.begin(); it < _winToolkits.end(); ++it) {
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

