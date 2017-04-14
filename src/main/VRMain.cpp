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
#include <main/VREventInternal.h>
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
	_factory->registerItemType<VRDisplayNode, VRStereoNode>("VRStereoNode");
	_factory->registerItemType<VRDisplayNode, VRViewportNode>("VRViewportNode");
	_factory->registerItemType<VRDisplayNode, VRLookAtNode>("VRLookAtNode");
	_factory->registerItemType<VRDisplayNode, VRTrackedLookAtNode>("VRTrackedLookAtNode");
	_factory->registerItemType<VRDisplayNode, VRViewportNode>("VRViewportNode");
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


bool fileGood(const std::string &fileName) {
    std::ifstream infile(fileName.c_str());
    return infile.good();
}
    
void VRMain::loadInstalledConfiguration(const std::string &configName) {
    std::string fname;
    std::vector< std::string > checked;
    
    // 1. current working directory
    fname = "./" + configName + ".minvr";
    checked.push_back(fname);
    if (fileGood(fname)) {
        loadConfigFile(fname);
        return;
    }
    
    // 2. config subdir within current working directory
    fname = "./config/" + configName + ".minvr";
    checked.push_back(fname);
    if (fileGood(fname)) {
        loadConfigFile(fname);
        return;
    }
    
    // 3. running from within the build tree from build/bin or tests-*/testname
    fname = "../../config/" + configName + ".minvr";
    checked.push_back(fname);
    if (fileGood(fname)) {
        loadConfigFile(fname);
        return;
    }
    
    // 4. an installed version based on MINVR_ROOT envvar
    if (std::getenv("MINVR_ROOT")) {
        fname = std::string(std::getenv("MINVR_ROOT")) + "/config/" + configName + ".minvr";
        checked.push_back(fname);
        if (fileGood(fname)) {
            loadConfigFile(fname);
            return;
        }
    }
    
    // 5. an installed version based on the INSTALL_PREFIX set with cmake
    fname = std::string(INSTALLPATH) + "/config/" + configName + ".minvr";
    checked.push_back(fname);
    if (fileGood(fname)) {
        loadConfigFile(fname);
        return;
    }
    
    std::cerr << "MinVR Error: Cannot find a configuration named " << configName
              << " in the following " << checked.size() << " locations:" << std::endl;
    for (int i=0; i<checked.size(); i++) {
        std::cerr << i+1 << ": " << checked[i] << std::endl;
    }
    throw std::runtime_error("MinVR Error: Cannot find a configuration named " + configName);
}

    
void VRMain::loadConfigFile(const std::string &pathAndFilename) {
    std::cout << "MinVR: Loading config file: " << pathAndFilename << std::endl;
    bool success = _config->processXMLFile(pathAndFilename,"/");
    if (!success) {
        throw std::runtime_error("MinVR Error: Could not process XML file " + pathAndFilename);
    }
}

    
    
void VRMain::setConfigValueByString(const std::string &keyAndValStr) {
    int poseql = keyAndValStr.find("=");
    if (poseql == std::string::npos) {
        throw std::runtime_error("MinVR Error: Expected a key=value format for the string: " + keyAndValStr);
    }
    std::string key = keyAndValStr.substr(0,poseql);
    std::string value = keyAndValStr.substr(poseql+1);
    
    std::cout << "MinVR: Setting config value: " << key << "=" << value << std::endl;

    // if the key already exists, then match the existing type, otherwise infer
    // the type as usual
    VRCORETYPE_ID type;
    if (_config->exists(key, "/")) {
        type = _config->getType(key);
    }
    else {
        type = _config->inferType(value);
    }
    
    
    if (type == VRCORETYPE_INT) {
        _config->addData(key, _config->deserializeInt(value.c_str()));
    }
    else if (type == VRCORETYPE_FLOAT) {
        _config->addData(key, _config->deserializeFloat(value.c_str()));
    }
    else if (type == VRCORETYPE_STRING) {
        _config->addData(key, value);
    }
    else if (type == VRCORETYPE_INTARRAY) {
        _config->addData(key, _config->deserializeIntArray(value.c_str(), MINVRSEPARATOR));
    }
    else if (type == VRCORETYPE_FLOATARRAY) {
        _config->addData(key, _config->deserializeFloatArray(value.c_str(), MINVRSEPARATOR));
    }
    else if (type == VRCORETYPE_STRINGARRAY) {
        _config->addData(key, _config->deserializeStringArray(value.c_str(), MINVRSEPARATOR));
    }
    //std::cout << _config->printStructure() << std::endl;
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
        std::cerr << "MinVR Error:  No VRSetups to start are defined" << std::endl;
        throw std::runtime_error("MinVR Error: No VRSetups to start are defined");
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
        std::cerr << "VRMain Error: The VRSetup " << _name << " has not loaded through a config file." << std::endl;
        throw std::runtime_error("MinVR Error: The VRSetup " + _name + " has not loaded through a config file.");
    }


    // for everything from this point on, the VRSetup name for this process is stored in _name, and this
	// becomes the base namespace for all of the VRDataIndex lookups that we do.


    // STEP 5: LOAD PLUGINS:

	// Load plugins from the plugin directory.  This will add their factories to the master VRFactory.
	

    // MinVR will try to load plugins based on a search path.  If it doesn't find the plugin
    // in one path, it will look in another supplied path.  To specify custom paths for an application
    // a user can set vrmain->addPLuginSearchPath(mypath);
    //
    // Here is the search path order that MinVR searches for plugins:
    //
    //    1. Plugin path specified in config ("/PluginPath" in VRDataIndex)
    //    2. Working directory (".")
    //    3. <Working directory>/plugins ("./plugins")
    //    4. Custom user defined paths (i.e. vrmain->addPluginSearchPath(mypath))
    //    5. <Binary directory>/../plugins ("build/bin/../plugins")
    //    6. <Install directory>/plugins ("install/plugins")
    //    7. <$MINVR_ROOT>/plugins ("$MINVR_ROOT/plugins")

    std::list<std::string> names = _config->selectByAttribute("pluginType", "*", _name);
    for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
        std::vector<std::string> pluginSearchPaths;
        if (_config->exists("PluginPath", *it)){
            std::string path = _config->getValue("PluginPath", *it);
            pluginSearchPaths.push_back(_config->dereferenceEnvVars(path));
        }
        pluginSearchPaths.push_back(".");
        pluginSearchPaths.push_back("./plugins");
        for (int f = 0; f < _pluginSearchPaths.size(); f++) {
            pluginSearchPaths.push_back(_pluginSearchPaths[f]);
        }
        std::string exe(argv[0]);
        std::size_t endPos = exe.find_last_of("/\\");
        std::string execPath = endPos != std::string::npos ? exe.substr(0,endPos) : ".";
        pluginSearchPaths.push_back(execPath + "/../plugins");
        pluginSearchPaths.push_back(std::string(INSTALLPATH) + "/plugins");
        const char* minvrRoot = std::getenv("MINVR_ROOT");
        if (minvrRoot) {
            pluginSearchPaths.push_back(std::string(minvrRoot) + "/plugins");
        }

        bool found = false;
        std::string file = "";
        string buildType = "";
#ifdef MinVR_DEBUG
        buildType = "d";
#endif

        for (std::vector<std::string>::const_iterator searchPath = pluginSearchPaths.begin(); searchPath != pluginSearchPaths.end(); ++searchPath) {
            file = _config->getDatum(*it)->getAttributeValue("pluginType");
            std::string path = *searchPath + "/" + file;

            if(_pluginMgr->loadPlugin(path, file + buildType)) {
                found = true;
                break;
            }
        }

        if (!found) {
            cerr << "VRMain Error: Problem loading plugin: " << file << buildType << endl;
            std::cout << "  Could not load from any of the following paths: " << std::endl;
            for (std::vector<std::string>::const_iterator searchPath = pluginSearchPaths.begin(); searchPath != pluginSearchPaths.end(); ++searchPath) {
                std::cerr << "\t"<< *searchPath << std::endl;
            }
        }

        //else if(_config->getDatum(*it)->hasAttribute("pluginlibfile")){
        //	std::string file = _config->getDatum(*it)->getAttributeValue("pluginlibfile");
        //	if (!_pluginMgr->loadPlugin(file)) {
        //		cerr << "VRMain Error: Problem loading plugin " << file << endl;
        //	}
        //}
    }


	// STEP 6: CONFIGURE NETWORKING:

	// check the type of this VRSetup, it should be either "VRServer", "VRClient", or "VRStandAlone"
	if(_config->getDatum(_name)->hasAttribute("hostType")){
		std::string type = _config->getDatum(_name)->getAttributeValue("hostType");
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
				std::cerr << "Problem creating inputdevice: " << *it << " with inputdeviceType=" << _config->getDatum(*it)->getAttributeValue("inputdeviceType") << std::endl;
			}
		}
	}

	// STEP 8: CONFIGURE WINDOWS
	{
		std::list<std::string> names = _config->selectByAttribute("displaynodeType", "*", _name);
		for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
			// CONFIGURE WINDOW TOOLKIT
			{
				int counttk = 0;
				std::string usedToolkit;
				std::list<std::string> namestk = _config->selectByAttribute("graphicstoolkitType", "*", *it);
				for (std::list<std::string>::reverse_iterator ittk = namestk.rbegin(); ittk != namestk.rend(); ++ittk) {
					// create a new graphics toolkit for each one in the list
					VRGraphicsToolkit *tk = _factory->create<VRGraphicsToolkit>(this, _config, *ittk);
					if (tk) {
						if (counttk == 0){
							_config->addData(_config->validateNameSpace(*it) + "GraphicsToolkit", tk->getName());
							usedToolkit = *ittk;
							counttk++;
						}
						else
						{
							std::cerr << "Warning : Only 1 graphics toolkit can be defined for : " << *it << " using graphicstoolkitType=" << _config->getDatum(_config->validateNameSpace(*it) + "GraphicsToolkit")->getValueString() << " defined at " << usedToolkit << std::endl;
							delete tk;
							break;
						}

						bool exists = false;
						for (std::vector<VRGraphicsToolkit*>::iterator it_gfxToolkits = _gfxToolkits.begin(); it_gfxToolkits != _gfxToolkits.end(); ++it_gfxToolkits)
						{
							if ((*it_gfxToolkits)->getName() == tk->getName())
							{
								exists = true;
								delete tk;
								break;
							}
						}
						if(!exists) _gfxToolkits.push_back(tk);

					}
					else{
						std::cerr << "Problem creating graphics toolkit: " << *ittk << " with graphicstoolkit=" << _config->getDatum(*ittk)->getAttributeValue("graphicstoolkitType") << std::endl;
					}
				}
			}

			// CONFIGURE GRAPHICS TOOLKIT
			{
				int counttk = 0; 
				std::string usedToolkit;
				std::list<std::string> namestk = _config->selectByAttribute("windowtoolkitType", "*", *it);	
				for (std::list<std::string>::reverse_iterator ittk = namestk.rbegin(); ittk != namestk.rend(); ++ittk) {
					// create a new graphics toolkit for each one in the list
					VRWindowToolkit *tk = _factory->create<VRWindowToolkit>(this, _config, *ittk);
					if (tk) {
						if (counttk == 0){
							_config->addData(_config->validateNameSpace(*it) + "WindowToolkit", tk->getName());
							usedToolkit = *ittk;
							counttk++;
						} 
						else
						{
							std::cerr << "Warning : Only 1 window toolkit can be defined for: " << *it << " using windowtoolkitType=" << _config->getDatum(_config->validateNameSpace(*it) + "WindowToolkit")->getValueString() << " defined at " << usedToolkit << std::endl;
							delete tk;
							break;
						}

						bool exists = false;
						for (std::vector<VRWindowToolkit*>::iterator it_winToolkits = _winToolkits.begin(); it_winToolkits != _winToolkits.end(); ++it_winToolkits)
						{
							if ((*it_winToolkits)->getName() == tk->getName())
							{
								exists = true;
								delete tk;
								break;
							}
						}
						if (!exists) _winToolkits.push_back(tk);
					}
					else{
						std::cerr << "Problem creating window toolkit: " << *ittk << " with windowtoolkitType=" << _config->getDatum(*it)->getAttributeValue("windowtoolkitType") << std::endl;
					}
				}
			}

			// add window to the displayGraph list
			VRDisplayNode *dg = _factory->create<VRDisplayNode>(this, _config, *it);
			if (dg) {
				_displayGraphs.push_back(dg);
			}
			else{
				std::cerr << "Problem creating window: " << *it << " with windowType=" << _config->getDatum(*it)->getAttributeValue("windowType") << std::endl;
			}
		}
	}

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
		std::string eventName = _config->addSerializedValue( events->getSerializedObject() );

        VREventInternal event(eventName, _config);
      
		// Invoke the user's callback on the new event
		for (int f = 0; f < _eventHandlers.size(); f++) {
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

