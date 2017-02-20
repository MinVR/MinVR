#ifndef VRMAIN_H
#define VRMAIN_H

#include <plugin/VRPluginManager.h>

#include <config/VRDataIndex.h>
#include <display/VRDisplayNode.h>
#include <display/VRGraphicsToolkit.h>
#include <display/VRWindowToolkit.h>
#include <input/VRInputDevice.h>
#include <main/VREventHandler.h>
#include <main/VRFactory.h>
#include <main/VRMainInterface.h>
#include <main/VRRenderHandler.h>
#include <net/VRNetInterface.h>
#include <main/VRAppLauncher.h>

namespace MinVR {



/** Advanced application programmers who require more flexibility than what is
    provided via api/VRApp should use this class as the main interface to the
    MinVR library.  The comments in the code are organized so as to walk the
    programmer through a set of 5 steps needed to add MinVR to an existing
    graphics program or get started with writing a new program.
*/
class VRMain : public VRMainInterface {
public:

    VRMain();
    virtual ~VRMain();


    /***** STEP 1:  REGISTER CALLBACKS FOR YOUR PROGRAM WITH MINVR *****/

    /** Register your own class that implements the VREventHandler interface
        in order to receive MinVR event callbacks.  You can register more
        than one handler, and they will be called in the order they were
        registered.
     */
    void addEventHandler(VREventHandler *eHandler);

    // TODO: Add function:  void removeEventHandler();

    /** Register your own class that implements the VRRenderHandler interface
        in order to recieve MinVR renderScene (and all other rendering-related)
        callbacks.  You can register more than one handler, and they will be
        called in the order they were registered.
     */
    void addRenderHandler(VRRenderHandler *rHandler);

    // TODO: Add function:  void removeRenderHandler();


    
    // STEP 2:  INITIALIZE MINVR BASED ON CONFIG FILE SETTINGS
  
    /** STEP 2 (option 1): The MinVR initialize step loads MinVR configuration
        files, spawns additional sub-processes (in the case of clustered VR
        setups), and creates any InputDevices and DisplayDevices specified in 
        the config files.
     
        Two initialize*() functions are provided.  Use this one,
        VRMain::initializeWithMinVRCommandLine(..), if you wish to use MinVR's
        convention for command line arguments.
     
        -h, --help         Display a help message.
     
        Add any of the following arguments to the command line as many times as
        needed in a space separated list.
     
        <configname>       Search for and load the pre-installed MinVR config file
                           named <configname>.minvr -- the search looks in:
                           1. the current working directory [cwd]
                           2. [cwd]/config
                           3. ../../config (for developers running build tree
                             executables from build/bin or tests/testname
                           4. MINVR_ROOT/config if the MINVR_ROOT envvar is defined
                           5. the install_prefix specified when libMinVR was built.
     
        <path/file.minvr>  Load the exact MinVR config file specified as a complete
                           relative or absolute path and filename.  The filename
                           must include a .minvr extension to distinguish this
                           option from loading a pre-installed configuration.
     
        <key>=<value>      Add an entry to the MinVR configuration directly from
                           the command line rather than by specifying it in a
                           config file. This can be used to override one specific
                           option in a pre-installed configuration or config file
                           specified earlier on the command line.  For example,
                           "myprogram desktop WindowHeight=500 WindowWidth=500"
                           would start myprogram, load the installed desktop MinVR
                           config and then override the WindowHeight and
                           WindowWidth values specified in the pre-installed
                           desktop configuration with the new values specified.
     
        [nothing]          If no command line arguments are provided, then MinVR
                           will try to load the pre-installed default
                           configuration, whis is the same as running the command
                           "myprogram default".
     
        MINVR_DATA=xxxxx   A special command line argument reserved for internal
                           use by MinVR.
     */
    void initializeWithMinVRCommandLineParsing(int argc, char **argv);
    
    
    /** STEP 2 (option 2): The MinVR initialize step loads MinVR configuration
        files, spawns additional sub-processes (in the case of clustered VR
        setups), and creates any InputDevices and DisplayDevices specified in 
        the config files.
     
        Two initialize*() functions are provided.  Use this one if you wish to
        load configufreation files yourself and do your own command line 
        parsing.
     
        With MinVR, you may use whatever convention you wish for command line 
        arguments as long as you allow for the last argument passed to your 
        program to be a special string used by MinVR.  This string will be 
        formated as follows:  MINVR_DATA=xxxxxx where xxxxxx is a coded string 
        of data with no spaces.  If you parse your own command line arguments, 
        we recommend that you do whatever parsing you like ignoring this string.  
        Then, simply forward the original argc and argv passed into your program 
        to MinVR::initialize().  MinVR will, in turn, ignore all of your command 
        line arguments and just pay attention to its special MINVR_DATA=xxxxxx 
        string.
     
        To tell MinVR which configuration files to load and any additional 
        key=value config settings that you with to apply, call the following 
        three functions.
     
         void VRMain::loadInstalledConfiguration(const std::string &configName);
         void VRMain::loadConfigFile(const std::string &pathAndFilename);
         void VRMain::setConfigValue(const std::string &key, const std::string &value);
     
        First, call any of these three functions in any order and as many times 
        as you wish, *then* call initialize(argc,argv). For example:
     
         VRMain *vrmain = new VRMain();
         vrmain->loadInstalledConfiguration("Display_3DTV");
         vrmain->loadInstalledConfiguration("WinToolkit_GLFW");
         vrmain->loadInstalledConfiguration("GfxToolkit_OpenGL");
         vrmain->loadConfigFile("/users/dan/myprogram/my-overrides.minvr");
         vrmain->setConfigValue("StereoFormat", "Side-by-Side");
         vrmain->initialize(argc, argv);
         
         while (vrmain->mainloop()) {}
         
         vrmain->shutdown();
     */
    void initializeWithUserCommandLineParsing(int argc, char **argv);
    
    /** For use before calling initializeWIthUserCommandLineParsing(..).
        This will search for and load the pre-installed MinVR config file named 
        <configName>.minvr -- the search looks in:
        1. the current working directory [cwd]
        2. [cwd]/config
        3. ../../config (for developers running build tree
           executables from build/bin or tests/testname
        4. MINVR_ROOT/config if the MINVR_ROOT envvar is defined
        5. the install_prefix specified when libMinVR was built.
     */
    void loadInstalledConfiguration(const std::string &configName);

    /** For use before calling initializeWIthUserCommandLineParsing(..).
        This will load a specific config file specified by a complete path
        and filename including .minvr extension.
     */
    void loadConfigFile(const std::string &pathAndFilename);

    /** For use before calling initializeWIthUserCommandLineParsing(..).
        This can be used to set a specific config key=value setting for MinVR
        directly from code rather than reading the setting in from a file.
     */
    void setConfigValue(const std::string &key, const std::string &value);
    
    


    
    /***** STEP 3: CALL MINVR'S MAINLOOP() FUNCTION *****/
    
    /** STEP 3 (option 1): Call this mainloop() function once per frame from
        your program's existing main loop.

        The first part of mainloop() is synchronizeAndProcessEvents().
        This will: (1) gather input events generated on the MinVR server and
        client(s), (2) synchronize these events so that every node has the
        same list of events to process for each frame, (3) process any events
        used internally by MinVR (e.g., headtracking), and (4) call any event
        handlers that have been registered with MinVR.

        The second part of mainloop() is renderOnAllDisplays(). This tells MinVR
        to traverse its DisplayGraph structure, updateing the appropriate 
        window, viewport, and other settings specified in the display nodes.  
        As MinVR traverses the DisplayGraph, it calls any render handlers 
        that have been registered with MinVR when it reaches an appropriate 
        node.
     */
    bool mainloop() {
        synchronizeAndProcessEvents();
        renderOnAllDisplays();
        return (!_shutdown);
    }

    /** STEP 3 (option 2, part a):  If you need more control, you can call
        synchronizeAndProcessEvents() then renderingOnAllDisplays() yourself
        rather than calling mainloop().
     */
    void synchronizeAndProcessEvents();

    /** STEP 3 (option 2, part b):  If you need more control, you can call
        synchronizeAndProcessEvents() then renderingOnAllDisplays() yourself
        rather than calling mainloop().
     */
    void renderOnAllDisplays();

    

    /***** STEP 4: SHUTDOWN MINVR *****/
    
    /** STEP 4: Call shutdown() to close any network connections and/or other
        resources created by MinVR
     */
    void shutdown();


    
    /***** USED INTERNALLY BY MINVR *****/
    
    std::string getName() { return _name; }

    VRFactory* getFactory() { return _factory; }

    VRDataIndex* getConfig() { return _config; }

    void addInputDevice(VRInputDevice* dev);
 
    VRGraphicsToolkit* getGraphicsToolkit(const std::string &name);
    VRWindowToolkit* getWindowToolkit(const std::string &name);
    void addPluginSearchPath(const std::string& path) {}

    std::list<std::string> auditValuesFromAllDisplays();
  
private:

    void processCommandLineArgs(std::string commandLine);
    void initializeInternal(int argc, char **argv);
    
    bool _initialized;

    std::string      _name;
    VRDataIndex*     _config;
    VRNetInterface*  _net;
    VRFactory*       _factory;
    VRPluginManager* _pluginMgr;

    std::vector<VREventHandler*>    _eventHandlers;
    std::vector<VRRenderHandler*>   _renderHandlers;

    std::vector<VRInputDevice*>     _inputDevices;
    std::vector<VRGraphicsToolkit*> _gfxToolkits;
    std::vector<VRWindowToolkit*>   _winToolkits;
    std::vector<VRDisplayNode*>     _displayGraphs;
    std::vector<std::string>        _pluginSearchPaths;

    int _frame;
    bool _shutdown;
};


} // end namespace

#endif
