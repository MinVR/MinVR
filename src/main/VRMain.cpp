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
#include <display/VRLookAtNode.h>
#include <display/VRTrackedLookAtNode.h>
#include <input/VRFakeTrackerDevice.h>
#include <net/VRNetClient.h>
#include <net/VRNetServer.h>
#include <plugin/VRPluginManager.h>
#include <sstream>
#include <main/VRLocalAppLauncher.h>
#include <cstdlib>

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

	virtual void onVRRenderScene(const VRDataIndex &renderState) {
		for (std::vector<VRRenderHandler*>::iterator it = _handlers.begin(); it != _handlers.end(); it++) {
			(*it)->onVRRenderScene(renderState);
		}
	}

	virtual void onVRRenderContext(const VRDataIndex &renderState) {
		for (std::vector<VRRenderHandler*>::iterator it = _handlers.begin(); it != _handlers.end(); it++) {
			(*it)->onVRRenderContext(renderState);
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
	_factory->registerItemType<VRDisplayNode, VRStereoNode>("VRStereoNode");
	_factory->registerItemType<VRDisplayNode, VRViewportNode>("VRViewportNode");
	_factory->registerItemType<VRDisplayNode, VRLookAtNode>("VRLookAtNode");
	_factory->registerItemType<VRDisplayNode, VRTrackedLookAtNode>("VRTrackedLookAtNode");
  _factory->registerItemType<VRInputDevice, VRFakeTrackerDevice>("VRFakeTrackerDevice");
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


void VRMain::loadInstalledConfiguration(const std::string &configName) {

  VRSearchConfig configPath;

  std::string fileName = configPath.findFile(configName);
  if (fileName.empty()) {

    VRERROR("Cannot find a configuration named " + configName + ".",
            "Checked: " + configPath.getPath());
  } else {

#ifdef MinVR_DEBUG
    std::cerr << "Loading configuration: " << fileName << std::endl;
#endif
    loadConfigFile(fileName);

  }
}


void VRMain::loadConfigFile(const std::string &pathAndFilename) {

    bool success = _config->processXMLFile(pathAndFilename,"/");
    if (!success) {
        throw std::runtime_error("MinVR Error: Could not process XML file " + pathAndFilename);
    }
}

void VRMain::setConfigValueByString(const std::string &keyAndValStr) {
  std::string name = _config->addData(keyAndValStr);
}


void VRMain::displayCommandLineHelp() {
    std::cout <<
    "-h, --help         Display this help message.\n"
    "\n"
    "Add any of the following arguments to the command line as many times as\n"
    "needed in a space separated list.\n"
    "\n"
    "-c <configname>, --load-config <configname>\n"
    "                   Search for and load the pre-installed MinVR config file\n"
    "                   named <configname>.minvr -- the search looks in:\n"
    "                   1. the current working directory [cwd]\n"
    "                   2. [cwd]/config\n"
    "                   3. ../../config (for developers running build tree\n"
    "                      executables from build/bin or tests/testname\n"
    "                   4. MINVR_ROOT/config if the MINVR_ROOT envvar is defined\n"
    "                   5. the install_prefix specified when libMinVR was built.\n"
    "\n"
    "-f <path/file.minvr>, --load-file <path/file.minvr>\n"
    "                   Load the exact MinVR config file specified as a complete\n"
    "                   relative or absolute path and filename.\n"
    "\n"
    "-s <key>=<value>, --set-value <key>=<value>\n"
    "                   Add an entry to the MinVR configuration directly from\n"
    "                   the command line rather than by specifying it in a\n"
    "                   config file. This can be used to override one specific\n"
    "                   option in a pre-installed configuration or config file\n"
    "                   specified earlier on the command line.  For example,\n"
    "                   'myprogram -c desktop -s WindowHeight=500 -s WindowWidth=500'\n"
    "                   would start myprogram, load the installed desktop MinVR\n"
    "                   config and then override the WindowHeight and\n"
    "                   WindowWidth values in the pre-installed desktop\n"
    "                   configuration with the new values specified.\n"
    "\n"
    "[nothing]          If no command line arguments are provided, then MinVR\n"
    "                   will try to load the pre-installed default\n"
    "                   configuration, whis is the same as running the command\n"
    "                   'myprogram --load-config default'.\n"
    "\n"
    "[anything else]    MinVR will silently ignore anything else provided as\n"
    "                   a command line option.\n"
    "\n"
    "--MINVR_DATA=xxxx  A special command line argument reserved for internal\n"
    "                   use by MinVR.\n"
    << std::endl;
}


void VRMain::processCommandLineArgs(std::string commandLine)  {

    std::stringstream argStream(commandLine);
    int count = 0;
    bool processeddata = false;

    while (argStream) {
        std::string arg;
        argStream >> arg;
        if (argStream) {
            count++;

            bool got_help   = ((arg == "-h") || (arg == "--help"));

            bool got_config = ((arg == "-c") || (arg == "--load-config"));

            bool got_file   = ((arg == "-f") || (arg == "--load-file"));

            bool got_keyval = ((arg == "-s") || (arg == "--set-value"));

            //bool got_envvar = ((arg == "-e") || (arg == "--set-envvar"));

            bool got_data   = (arg.find("--MINVR_DATA=") == 0);



            // case 0: display help and exit
            if (got_help) {
                displayCommandLineHelp();
                exit(0);
            }

            // case 1: a pre-installed MinVR configuration <configname>.minvr
            else if (got_config) {
                argStream >> arg;
                loadInstalledConfiguration(arg);
            }

            // case 2: a specific path/filename.minvr config file to load
            else if (got_file) {
                argStream >> arg;
                loadConfigFile(arg);
            }

            // case 3: a key=value pair to add to the config
            else if (got_keyval) {
                argStream >> arg;
                setConfigValueByString(arg);
            }

            /*
            // case 4: a varname=value to set as an environment variable
            else if (got_envvar) {
                argStream >> arg;
                int poseql = arg.find("=");
                if (poseql == std::string::npos) {
                    throw std::runtime_error("MinVR Error: Cannot set value, expected to find an = sign on the command line.");
                }
                std::string key = arg.substr(0,poseql);
                std::string value = arg.substr(poseql+1);
                // TODO: ????  setenv(key.c_str(), value.c_str(), ??);
            }
            */

            // case 5: the special --MINVR_DATA=xxxx flag
            else if (got_data) {
                int posdata = arg.find("--MINVR_DATA=");
                std::string data = arg.substr(posdata+13);
                std::string decoded = VRAppLauncher::dataToArgs(data);
                // recursive call to process arguments encoded in MINVR_DATA
                processCommandLineArgs(decoded);
                processeddata = true;
            }
        }
    }

    // If there were no command line arguments or if the only command line
    // argument was the special --MINVR_DATA=xxxx argument, then load the
    // pre-installed default configuration.
    if ((count == 0) || ((count == 1) && (processeddata))) {
        loadInstalledConfiguration("default");
    }
}




void VRMain::initializeWithMinVRCommandLineParsing(int argc, char **argv) {
    // build a single string with all initialization commands separated by spaces
    std::string cmdline;
    for (int i=1; i<argc; i++) {
        if (i>1) {
            cmdline += " ";
        }
        cmdline += argv[i];
    }
    processCommandLineArgs(cmdline);

    initializeInternal(argc, argv);
}



void VRMain::initializeWithUserCommandLineParsing(int argc, char **argv) {
    // In this case, ignore all the command line arguments except for
    // one of the format --MINVR_DATA=xxxx.  If present, this must be a child
    // process and the parent is passing its config data to us.
    std::string minvrData;
    for (int i=1; i<argc; i++) {
        std::string argstr(argv[i]);
        int posdata = argstr.find("--MINVR_DATA=");
        if (posdata != std::string::npos) {
            // process just this one --MINVR_DATA=xxxx argument
            processCommandLineArgs(argstr);
        }
    }

    initializeInternal(argc, argv);
}




void VRMain::initializeInternal(int argc, char **argv) {

	VRStringArray vrSetupsToStartArray;
	if (!_config->exists("VRSetupsToStart","/")) {
		// no vrSetupsToStart are specified, start all of VRSetups listed in the config file
		std::list<std::string> names = _config->selectByAttribute("hostType","*");
		for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
			vrSetupsToStartArray.push_back(*it);
		}
	}
	else {
		// a comma-separated list of vrSetupsToStart was provided
		std::string vrSetupsToStart = _config->getValue("VRSetupsToStart","/");
		VRString elem;
		std::stringstream ss(vrSetupsToStart);
		while (std::getline(ss, elem, ',')) {
			vrSetupsToStartArray.push_back(elem);
		}
	}


	if (vrSetupsToStartArray.empty()) {
    VRERROR("No VRSetups to start are defined.", "Your config file must contain at least one VRSetup element.");
		exit(1);
	}


    // STEP 1: IF ANY OF THE VRSETUPS ARE STARTED AS REMOTE PROCESSES ON ANOTHER
    // MACHINE, THEN START THEM NOW AND REMOVE THEM FROM THE LIST OF VRSETUPS TO
    // START ON THIS MACHINE

    vector<std::string>::iterator it = vrSetupsToStartArray.begin();
    while (it != vrSetupsToStartArray.end()) {
        if (_config->exists("HostIP",*it) && !_config->exists("StartedSSH","/")) {

            // TODO: If we're going to use VRAppLaunchers, then this seems like
            // it should be implemented in a VRSSHAppLauncher class
            VRLocalAppLauncher launcher(argc, argv);

            // Setup needs to be started via ssh.
            // First get the path were it has to be started
            std::string workingDirectory = getCurrentWorkingDir();
            std::string nodeIP = _config->getValue("HostIP",*it);
            std::string command = "cd " + workingDirectory + ";";
            if (_config->exists("HostDisplay",*it)) {
                std::string displayVar = _config->getValue("HostDisplay",*it);
                command = command + "DISPLAY=" + displayVar + " ";
            }

            std::string processSpecificArgs = "VRSetupsToStart=" + *it + " StartedSSH=1";

            std::string sshcmd;
            if (_config->exists("LogToFile",*it)) {
                std::string logFile = _config->getValue("LogToFile",*it);
                sshcmd = "ssh " + nodeIP + " '" + command + launcher.generateCommandLine(processSpecificArgs) + " > " + logFile + " " +  "2>&1 &'";
            }
            else {
                sshcmd = "ssh " + nodeIP + " '" + command + launcher.generateCommandLine(processSpecificArgs) + " > /dev/null 2>&1 &'";
                //sshcmd = "ssh " + nodeIP + " '" + command + launcher.generateCommandLine(sshData) + "'";
            }

            std::cerr << "Start " << sshcmd << std::endl;

            // we start and remove all clients which are started remotely via ssh
            it = vrSetupsToStartArray.erase(it);
            system(sshcmd.c_str());
        }else{
            //setup does not need to be started remotely or was already started remotely
            ++it;
        }
    }

	if (vrSetupsToStartArray.empty()) {
        std::cout << "MinVR: All VRSetups are remote processes. All have been started via SSH - Exiting" << std::endl;
		exit(1);
	}


    // STEP 2: RECORD THE NAME OF THE VRSETUP TO USE FOR THIS PROCESS

	// This process will be the first one listed
	_name = vrSetupsToStartArray[0];


    // STEP 3: IF ANY MORE VRSETUPS ARE SUPPOSED TO BE STARTED ON THIS MACHINE
    // THEN DO THAT NOW -- UNFORTUNATELY, THE WAY TO DO THIS ON WINDOWS IS
    // DIFFERENT THAN OTHER ARCHITECTURES.

	// Fork a new process for each remaining vrsetup

#ifdef WIN32
	// Windows doesn't support forking, but it does allow us to create processes,
	// so we just create a new process with the config file to load as the first
	// command line argument and the vrsetup to start as the second command line
	// argument -- this means we need to enforce this convention for command line
	// arguments for all MinVR programs that want to support multiple processes.

    VRLocalAppLauncher launcher(argc,argv);

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

        std::string processSpecificArgs = "VRSetupsToStart=" + vrSetupsToStartArray[i];
		std::string cmdLine = launcher.generateCommandLine(processSpecificArgs);

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
			throw std::runtime_error("MinVR Error: CreateProcess failed: " + GetLastError());
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


    // STEP 4:  Sanity check to make sure the vrSetup we are continuing with is
    // actually defined in the config settings that have been loaded.
	if (!_config->exists(_name)) {
    VRERROR("VRMain Error: The VRSetup " +
            _name + " has not loaded through a config file.",
            "Your config file must contain a VRSetup element.");
    }

    // for everything from this point on, the VRSetup name for this process is stored in _name, and this
	// becomes the base namespace for all of the VRDataIndex lookups that we do.


    // STEP 5: LOAD PLUGINS:

	// Load plugins from the plugin directory.  This will add their
	// factories to the master VRFactory.


  // MinVR will try to load plugins based on a search path.  If it
  // doesn't find the plugin in one path, it will look in another
  // supplied path.  To specify custom paths for an application, use
  // addPLuginSearchPath().  See VRSearchPath for more information.
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

	// check the type of this VRSetup, it should be either "VRServer", "VRClient", or "VRStandAlone"
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
VRMain::synchronizeAndProcessEvents() {

	if (!_initialized) {
		throw std::runtime_error("VRMain not initialized.");
	}

  VRDataQueue eventQueue;

  // Add a standard "FrameStart" event at the beginning of each frame
  VRDataIndex frameStartEvent("FrameStart");
  frameStartEvent.addData("ElapsedSeconds", (float)VRSystem::getTime());
  frameStartEvent.addData("EventType", "AnalogChange");
  frameStartEvent.linkNode("ElapsedSeconds", "DefaultData");
  eventQueue.push(frameStartEvent.serialize());

  for (int f = 0; f < _inputDevices.size(); f++) {
    _inputDevices[f]->appendNewInputEventsSinceLastCall(&eventQueue);
  }

	// SYNCHRONIZATION POINT #1: When this function returns, we know
	// that all MinVR nodes have the same list of input events generated
	// since the last call to synchronizeAndProcessEvents(..).  So,
	// every node will process the same set of input events this frame.
  if (_net != NULL) {
    eventQueue = _net->syncEventDataAcrossAllNodes(eventQueue);
  }

  while (eventQueue.notEmpty()) {
    // Unpack the next item from the queue and invoke the user's
    // callback on it.
    for (int f = 0; f < _eventHandlers.size(); f++) {
      _eventHandlers[f]->onVREvent(eventQueue.getFirst());
    }

    // Remove the item from the queue.
    eventQueue.pop();
  }

  // At this point the eventQueue should be empty with all its events
  // distributed to the user callback.  Our job here is done and we can
  // safely get out.
}

void VRMain::renderOnAllDisplays() {

  if (!_initialized) throw std::runtime_error("VRMain not initialized.");

	VRDataIndex renderState;
    renderState.setName("RenderState");
	renderState.addData("InitRender", (int)(_frame == 0));

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

