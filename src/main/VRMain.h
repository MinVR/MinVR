#ifndef VRMAIN_H
#define VRMAIN_H

#include <plugin/VRPluginManager.h>

#include <config/VRDataIndex.h>
#include <display/VRDisplayNode.h>
#include <display/VRGraphicsToolkit.h>
#include <display/VRWindowToolkit.h>
#include <input/VRInputDevice.h>
#include <api/VREventHandler.h>
#include <main/VRFactory.h>
#include <main/VRMainInterface.h>
#include <api/VRRenderHandler.h>
#include <net/VRNetInterface.h>
#include <main/VRAppLauncher.h>

namespace MinVR {



/** Application programmers should use this class as the main interface to the
    MinVR library.  The comments in the code are organized so as to walk the
    programmer through a set of 5 steps needed to add MinVR to an existing
    graphics program or get started with writing a new program.
*/
class VRMain : public VRMainInterface {
public:

  VRMain();
  virtual ~VRMain();


  // FOLLOW THE 4 STEPS BELOW TO CREATE A MINVR PROGRAM!

  // STEP 1:  REGISTER CALLBACKS FOR YOUR PROGRAM WITH MINVR

  /// Register your own class that implements the VREventHandler interface
  /// in order to receive MinVR event callbacks.  You can register more
  /// than one handler, and they will be called in the order they were
  /// registered.
  void addEventHandler(VREventHandler *eHandler);

  // TODO: Add function:  void removeEventHandler();


  /// Register your own class that implements the VRRenderHandler interface
  /// in order to recieve MinVR renderScene (and all other rendering-related)
  /// callbacks.  You can register more than one handler, and they will be
  /// called in the order they were registered.
  void addRenderHandler(VRRenderHandler *rHandler);

  // TODO: Add function:  void removeRenderHandler();


  // STEP 2:  INITIALIZE MINVR BASED ON CONFIG FILE SETTINGS
  
  /// Convention is that the first command line argument specifies the config
  /// file to load and the optional second command line argument specifies a
  /// comma separated list of vr setups to start (assuming that more than one
  /// vrsetup is listed in the config file).  If MinVR is instructed to start
  /// more than one vrsetup then it will fork (on linux and OSX systmes) or
  /// create a new process (on windows systems) as necessary so that each vrsetup
  /// has its own process.
  void initialize(int argc, char **argv);
  /// If a user wants more control of the command line, this initialize method is the one
  /// that should be used.  Here a user can provide any set of command line arguments in any order.
  /// The only required item that also needs to be passed is the configFile path.  Optionally, a
  /// user can also provide data index overrides in the format "/path/to/item=value".
  void initialize(int argc, char **argv, const std::string& configFile, std::vector<std::string> dataIndexOverrides = std::vector<std::string>());
  /// If a user wants to control everything about the command line including how MinVR parameters
  /// are passed to forked processes, this initialize method should be used.  This is a more advanced
  /// feature and a user should refer to VRAppLauncher and the default implementation in VRDefaultAppLauncher
  void initialize(const VRAppLauncher& launcher);
  

  // STEP 3:  CALL MINVR'S MAINLOOP() FUNCTION OR INCORPORATE THE FOLLOWING
  // TWO CALLS INTO YOUR PROGRAM'S EXISTING MAINLOOP
  void mainloop() {
    synchronizeAndProcessEvents();
    renderOnAllDisplays();
  }

  // STEP 3a:  Typically this is the first call in your program's mainloop.
  // First, MinVR syncs events across all VR nodes.  Then, MinVR
  // processes them to update its own state (e.g., for headtracking).
  // Finally, MinVR calls your event callback function so that your
  // program can also respond to input events.
  void synchronizeAndProcessEvents();


  // STEP 3b: Tell MinVR to traverse its DisplayGraph structure, updateing
  // the appropriate window, viewport, and other settings specified in
  // the display nodes.  As MinVR traverses the DisplayGraph, it calls
  // the render callbacks that were registered when it reaches an
  // appropriate node.
  void renderOnAllDisplays();



  // STEP 4:  Call shutdown() to close any network connections and/or other
  // resources created by MinVR
  void shutdown();


  std::string getName() { return _name; }

  VRFactory* getFactory() { return _factory; }

  VRDataIndex* getConfig() { return _config; }

  void addInputDevice(VRInputDevice* dev);
 
  VRGraphicsToolkit* getGraphicsToolkit(const std::string &name);
  VRWindowToolkit* getWindowToolkit(const std::string &name);
  void addPluginSearchPath(const std::string& path) {

  }
   
private:

  void initialize();
  bool _initialized;

  std::string      _name;
  VRDataIndex*     _config;
  VRNetInterface*  _net;
  VRFactory*       _factory;
  VRPluginManager* _pluginMgr;

  std::vector<VREventHandler*>  _eventHandlers;
  std::vector<VRRenderHandler*> _renderHandlers;

  std::vector<VRInputDevice*>     _inputDevices;
  std::vector<VRGraphicsToolkit*> _gfxToolkits;
  std::vector<VRWindowToolkit*>   _winToolkits;
  std::vector<VRDisplayNode*>     _displayGraphs;
  std::vector<std::string> _pluginSearchPaths;

  int _frame;
};


} // end namespace

#endif
