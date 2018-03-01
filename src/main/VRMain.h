#ifndef VRMAIN_H
#define VRMAIN_H

#include <set>
#include <plugin/VRPluginManager.h>

#include <config/VRDataIndex.h>
#include <display/VRDisplayNode.h>
#include <display/VRGraphicsToolkit.h>
#include <display/VRWindowToolkit.h>
#include <input/VRInputDevice.h>
#include <main/VRFactory.h>
#include <main/VRMainInterface.h>
#include <net/VRNetInterface.h>
#include <config/base64/base64.h>
#include <main/VREventHandler.h>
#include <main/VRRenderHandler.h>
#include <main/VRModelHandler.h>
#include <main/VRError.h>
#include <main/VRSearchPath.h>

namespace MinVR {

/// \brief Parses a couple of things out of the command line, leaves the
/// rest.
///
/// This virtual class is used to get a couple of MinVR-specific values out
/// of a command line.  Since the application programmer may also have
/// designs on the command line, there is some flexibility built in for his
/// or her benefit, including the ability to turn off the command-line
/// parsing entirely (almost), in which case the application program is
/// responsible for reading the configuration data.
///
/// The class supports selecting a small number of options out of a
/// command-line, specified with private member data values.  There are
/// mutators and accessors for these values, allowing the application
/// programmer to modify these values, if necessary to accommodate some
/// conflict.  You are responsible for providing the '--' or '-' in the new
/// strings, as necessary.
///
/// Command-line options can be of the form '-s value' or '-s=value', and it
/// supports short and long variants, so '-s' or '--set-value'.  (Since the
/// argument to '-s' should always have an equal sign, we discourage the use
/// of the equal sign form of '-s', but it works.)
///
/// One special functionality is '--MINVR_DATA=', where the argument to the
/// option contains an entire encoded command line.  If the parser
/// encounters this option, it unpacks the encoded string, and executes the
/// parser on this unpacked string, recursively.
///
/// This parser is implemented by sub-classing, and supplying the missing
/// member methods to execute the parse.
///
class VRParseCommandLine {

 public:
  VRParseCommandLine() {
    _setConfigValueShort = "-s";
    _setConfigValueLong = "--set-value";
    _loadConfigShort = "-c";
    _loadConfigLong = "--load-config";
    _helpShort = "-h";
    _helpLong = "--help";
    _noExecute = "--no-execute";
    _minVRData = "--MINVR_DATA";
    _execute = true;
  };

  // These two methods are the ultimate purpose of this class.  The
  // twin purposes of this class are to load the configuration data
  // easily, including command-line modifications of it -- and to
  // communicate configuration data from server to clients.

  /// \brief Accepts a key=value string and inserts it into the configuration.
  virtual void setConfigValue(const std::string &keyAndValStr) = 0;

  /// \brief Accepts the name of a configuration or configuration file.
  virtual void loadConfig(const std::string &configName) = 0;

  /// \brief If no command line arguments are provided to MinVR, then
  /// this function is called to load the default.minvr config file
  /// shipped with MinVR.
  virtual void loadDefaultConfig() = 0;


  /// \brief Turns off command-line parsing.
  ///
  /// An application program may choose not to use the MinVR-supplied
  /// command line arguments.  You can turn off checking for any of
  /// these options by setting the private data members to empty
  /// strings.  Use this function to turn them all off.  Note that you
  /// cannot turn off the --MINVR_DATA argument, which is used for
  /// servers and clients to communicate configuration options between
  /// themselves.
  ///
  /// Note that if you turn off the parsing, the MINVR_DATA business
  /// will still communicate *some* data between server and clients,
  /// but not necessarily all that you want or expect.  That is, the
  /// MINVR_DATA will be used to send configuration data from the
  /// server to the client, but it is your responsibility to read and
  /// use it on the receiving end.  Caveat programmor.
  void noParsing() {
    _setConfigValueShort = "";
    _setConfigValueLong = "";
    _loadConfigShort = "";
    _loadConfigLong = "";
    _helpShort = "";
    _helpLong = "";
    _noExecute = "";
  }

  /// \brief Parses the command line, per the options provided in the
  /// private variables.
  ///
  /// The return value indicates whether we are actually running the
  /// program, or just testing the startup with the noExecute option.  To be
  /// more specific, it only signals the absence/presence of the '-N'
  /// (noExecute) option.  (False if the -N is present.)
  ///
  /// The recursing argument is only to be used internally.
  bool parseCommandLine(int argc, char** argv, bool recursing = false);

  /// \brief Returns argc once the MinVR-specific args have been removed.
  int getLeftoverArgc() { return _leftoverArgc; };

  /// \brief Returns argv once the MinVR-specific args have been removed.
  char** getLeftoverArgv() { return _leftoverArgv; };

  /// \brief Returns the command-line abbreviation for setConfigValue.
  std::string getSetConfigValueShort() { return _setConfigValueShort; };

  /// \brief Changes the command-line abbreviation for setConfigValue.
  ///
  /// By default, this is "-s".
  void setSetConfigValueShort(const std::string &s) { _setConfigValueShort = s; };

  /// \brief Returns the command-line option for setConfigValue.
  std::string getSetConfigValueLong() { return _setConfigValueLong; };

  /// \brief Changes the command-line option for setConfigValue.
  ///
  /// By default, this is "--set-value".
  void setSetConfigValueLong(const std::string &s) { _setConfigValueLong = s; };

  /// \brief Returns the command-line abbreviation for loadConfig.
  std::string getLoadConfigShort() { return _loadConfigShort; };

  /// \brief Changes the command-line abbreviation for loadConfig.
  ///
  /// By default, this is "-c".
  void setLoadConfigShort(const std::string &s) { _loadConfigShort = s; };

  /// \brief Returns the command-line option for loadConfig.
  std::string getLoadConfigLong() { return _loadConfigLong; };

  /// \brief Changes the command-line option for loadConfig.
  ///
  /// By default, this is "--load-config".
  void setLoadConfigLong(const std::string &s) { _loadConfigLong = s; };

  /// \brief Returns the command-line abbreviation for help.
  std::string getHelpShort() { return _helpShort; };

  /// \brief Changes the command-line abbreviation for help.
  ///
  /// By default, this is "-h".
  void setHelpShort(const std::string &s) { _helpShort = s; };

  /// \brief Returns the command-line option for help.
  std::string getHelpLong() { return _helpLong; };

  /// \brief Changes the command-line option for help.
  ///
  /// By default, this is "--help".
  void setHelpLong(const std::string &s) { _helpLong = s; };

  /// \brief Returns the command-line option for specifying an encoded
  /// command-line.
  std::string getMinVRData() { return _minVRData; };

  /// \brief Changes the command-line option for MINVR_DATA.
  ///
  /// By default, this is "--MINVR_DATA".  You are strongly
  /// discouraged from changing this one, though it is possible.
  void setMinVRData(const std::string &s) { _minVRData = s; };

  /// \brief Collect some args into a base64-encoded string.
  std::string argsToData(const std::string &argStr);

  /// \brief Get some args from a base64-encoded string.
  std::string dataToArgs(const std::string &payload);

  /// \brief Returns the command-line option for help.
  std::string getNoExecute() { return _noExecute; };

  bool getExecute() { return _execute; };

  /// \brief Changes the command-line option for help.
  ///
  /// By default, this is "--help".
  void setNoExecute(const std::string &s) { _noExecute = s; };

  /// \brief Returns the original command line, with MinVR and non-MinVR args.
  std::string getOriginalCommandLine() { return _originalCommandLine; };

  /// \brief Returns the command line with the MinVR-relevant arguments left out.
  std::string getLeftoverCommandLine() { return _leftoverCommandLine; };

  /// \brief Returns the configuration path currently in use.
  VRSearchConfig getConfigPath() { return _configPath; };

  /// \brief Change the search path for configurations.
  ///
  /// This is used to find a configuration when it is specified on the
  /// command line.
  void setConfigPath(const VRSearchConfig &c) { _configPath = c; };

  /// \brief Prints a helpful message.
  void help(VRSearchConfig &configPath, const std::string additionalText = "") {
    std::cerr << makeHelpString(configPath, additionalText) << std::endl;
  }

  std::string makeHelpString(VRSearchConfig &configPath,
                             const std::string additionalText) {
    std::string out;
    out = getHelpShort() + ", " + getHelpLong() +
      "         Display this help message.\n\n" +
      "Add any of the following arguments to the command line as many times as\n" +
      "needed in a space separated list.\n\n" +
      getLoadConfigShort() + " <configName>, " +
      getLoadConfigLong() + " <configName>\n" +
      "                   Search for and load a MinVR config file.  If configName\n" +
      "                   has no suffix, MinVR looks for a file \n" +
      "                   named <configname>.minvr -- the search looks in:\n" +
      "                   " + configPath.getPath() +  "\n\n" +
      "                   You can also specify a config file as a complete relative\n" +
      "                   or absolute path and filename.\n\n" +
      getSetConfigValueShort() + " <key>=<value>, " +
      getSetConfigValueLong() + " <key>=<value>\n" +
      "                   Add an entry to the MinVR configuration directly from\n" +
      "                   the command line rather than by specifying it in a\n" +
      "                   config file. This can be used to override one specific\n" +
      "                   option in a pre-installed configuration or config file\n" +
      "                   specified earlier on the command line.  For example,\n" +
      "                   'myprogram -c desktop -s WindowHeight=500 -s WindowWidth=500'\n" +
      "                   would start myprogram, load the installed desktop MinVR\n" +
      "                   config and then override the WindowHeight and\n" +
      "                   WindowWidth values in the pre-installed desktop\n" +
      "                   configuration with the new values specified.\n\n" +
      getNoExecute() +
      "                Does not run the program, but describes what it would\n" +
      "                   do if you ran it.  Comparable to 'make -n'.\n\n" +
      getMinVRData() +
      "=xxxx  A special command line argument reserved for internal\n" +
      "                   use by MinVR.\n" +
      "[NO CONFIG ARGS]   MinVR cannot run without a configuration, so if no MinVR config\n" +
      "                   files or settings are specified on the command line, then the\n"
      "                   default.minvr config file shipped with MinVR is loaded.\n";

    if (additionalText.size() > 0) {
      out += additionalText;
    } else {

      out += std::string("[anything else]") +
        "    MinVR will silently ignore anything else provided as\n" +
        "                   a command line option and return it to the application program.\n\n";
    }
    return out;
  };

  /// \brief Processes a 'MINVR_DATA=' option.
  ///
  /// Decodes the payload to a 'MINVR_DATA=' option and calls
  /// parseCommandLine() on it recursively.
  void decodeMinVRData(const std::string &payload);

 private:

  VRSearchConfig _configPath;

  std::string _setConfigValueShort, _setConfigValueLong;
  std::string _loadConfigShort, _loadConfigLong;
  std::string _helpShort, _helpLong;
  std::string _noExecute;
  std::string _minVRData;

  std::string _originalCommandLine;
  std::string _leftoverCommandLine;

  /// Used to indicate whether this is to be executed, or if we are just testing.
  bool _execute;

  /// \brief This is argc once the MinVR-specific args have been removed.
  int _leftoverArgc;

  /// \brief This is argv once the MinVR-specific args have been removed.
  char* _leftoverArgv[50];

};









/** Advanced application programmers who require more flexibility than what is
    provided via api/VRApp should use this class as the main interface to the
    MinVR library.  The comments in the code are organized so as to walk the
    programmer through a set of 5 steps needed to add MinVR to an existing
    graphics program or get started with writing a new program.
*/
 class VRMain :
   public VRMainInterface,
   public VRParseCommandLine  {
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

    /** Register your own class that implements the VRModelHandler interface
        in order to be able to update the model without having to do it in
        the render handler or event handler.  You can register more
        than one handler, and they will be called in the order they were
        registered.
     */
    void addModelHandler(VRModelHandler* modelHandler);

    /** Register your own class that implements the VRRenderHandler interface
        in order to recieve MinVR renderScene (and all other rendering-related)
        callbacks.  You can register more than one handler, and they will be
        called in the order they were registered.
     */
    void addRenderHandler(VRRenderHandler *rHandler);

    // TODO: Add function:  void removeRenderHandler();



    // STEP 2:  INITIALIZE MINVR BASED ON CONFIG FILE SETTINGS

    /** STEP 2 The MinVR initialize step loads MinVR configuration
        files, spawns additional sub-processes (in the case of clustered VR
        setups), and creates any InputDevices and DisplayDevices specified in
        the config files.

        MinVR configuration values can be set via the command line, using
        command-line options specified via the VRParseCommandLine protocol.  You
        can specify either a file full of configuration options, or individual
        configuration values.  After initialize() is called, command-line
        options are accessible to your code via getLeftoverArgc() and
        getLeftoverArgv().

        You can redefine the command-line options for MinVR, or disable most of
        them, using the VRParseCommandLine methods, such as noParsing().  If you
        disable the configuration options, your code will have to tell MinVR
        which configuration files to load and any additional key=value config
        settings that you with to apply.  You can use the following two
        functions for this.

        void VRMain::loadConfig(const std::string &configName);
        void VRMain::setConfigValue(const std::string &key, const std::string &value);

        Call these two functions in any order and as many times as you wish,
        *then* call initialize(argc,argv).  For example:

         VRMain *vrmain = new VRMain();
         vrmain->loadConfig("Display_3DTV");
         vrmain->loadConfig("WinToolkit_GLFW");
         vrmain->loadConfig("GfxToolkit_OpenGL");
         vrmain->loadConfig("/users/dan/myprogram/my-overrides.minvr");
         vrmain->setConfigValue("StereoFormat", "Side-by-Side");
         vrmain->initialize(argc, argv);

         while (vrmain->mainloop()) {}

         vrmain->shutdown();

        Note that even when command line parsing is disabled, MinVR uses a
        command-line option for communication between MinVR processes.  You will
        see a "--MINVR_DATA" option appear on commands issued by a server to its
        clients.  When parsing is disabled, MinVR simply copies argc and argv
        via getLeftoverArgc() and getLeftoverArgv(), processing this option
        along the way.

     */
    void initialize(int argc, char **argv);

    /** Use this method to get the number of command line arguments that the
	      initialize() methods did not need.
    */
    int getArgc() { return getLeftoverArgc(); };
    /** Use this method to get the arguments on a command line that the
	      initialize() methods did not need.
    */
    char** getArgv() { return getLeftoverArgv(); };


    /** For use before calling initialize().  This will either load a given file
        name, or it will search for a file according to the VRSearchConfig
        rules.
     */
    void loadConfig(const std::string &pathAndFilename);

    /** For use before calling initialize().  This will either load the
        default.minvr config file shipped with MinVR.
     */
    void loadDefaultConfig();

    /** For use before calling initializeWIthUserCommandLineParsing(..).
        This can be used to set a specific config key=value setting for MinVR
        directly from code rather than reading the setting in from a file.
        The type is inferred automatically using the VRDataIndex.
        @param keyAndValStr A string of the form "key=value".
     */
    void setConfigValue(const std::string &keyAndValStr);


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
        updateAllModels();
        renderOnAllDisplays();
        return (!_shutdown);
    }

    /** STEP 3 (option 2, part a):  If you need more control, you can call
        synchronizeAndProcessEvents() then renderingOnAllDisplays() yourself
        rather than calling mainloop().
     */
    void synchronizeAndProcessEvents();

    /** STEP 3 (option 2, part b): You can update whatever models are used
        in your graphics here, and do so independently of the render functions.
    */
    void updateAllModels();

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




    /***** ACCESS TO MINVR DETAILS *****/

    /// Returns a data index containing all of the setup information loaded in
    /// to the program via config files.
    VRDataIndex* getConfig() { return _config; }

    /// Provides access to pointers to input devices based on the DeviceID,
    /// which is a data field within all the eventData for all MinVR events.
    /// Note that it's only possible to access locally attached input devices.
    /// DeviceID = -1 is used for events that come from networked nodes, and
    /// getInputDevice will return NULL when deviceID=-1.
    VRInputDevice* getInputDevice(int deviceID);

    /// Provides access to pointers to display nodes based on the NodeID,
    /// which is a data field within the renderState data passed by MinVR
    /// to all rendering callbacks.
    VRDisplayNode* getDisplayNode(int nodeID);




    /***** USED INTERNALLY BY MINVR -- THESE COULD PROBABLY BE MOVED TO AN IMPLEMENTATION FILE *****/

    std::string getName() { return _name; }

    VRFactory* getFactory() { return _factory; }


    void addInputDevice(VRInputDevice* dev);

    VRGraphicsToolkit* getGraphicsToolkit(const std::string &name);
    VRWindowToolkit* getWindowToolkit(const std::string &name);
    void addPluginSearchPath(const std::string& path) {
      _pluginSearchPath.addPathEntry(path, true);
    }
    void auditValuesFromAllDisplays();

    void displayCommandLineHelp();

    /// \brief Spawn a new process on a remote machine.
    ///
    /// This is for MinVR internal use, public only for testing.
    void _startSSHProcess(const std::string &name,
                          const bool noSSH=false);

    /// \brief Spawn a new process on the local machine.
    ///
    /// This is for MinVR internal use, public only for testing.
    bool _startLocalProcess(const std::string &name);

 private:

    VRSearchConfig _configPath;

    bool _initialized;

    int _argcRemnants;
    char** _argvRemnants;

    std::string      _name;
    VRDataIndex*     _config;
    VRNetInterface*  _net;
    VRFactory*       _factory;
    VRPluginManager* _pluginMgr;

    std::vector<VREventHandler*>    _eventHandlers;
    std::vector<VRModelHandler*>    _modelHandlers;
    std::vector<VRRenderHandler*>   _renderHandlers;

    std::vector<VRInputDevice*>     _inputDevices;
    std::vector<VRGraphicsToolkit*> _gfxToolkits;
    std::vector<VRWindowToolkit*>   _winToolkits;
    std::vector<VRDisplayNode*>     _displayGraphs;

    VRSearchPlugin                  _pluginSearchPath;

    int _frame;
    bool _shutdown;
};


} // end namespace

#endif
