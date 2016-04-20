#ifndef VRMAIN_H
#define VRMAIN_H

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
#include <plugin/VRPluginManager.h>

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

  /// If the configFile lists more than one VRSetup in it, then the vrSetups
  /// argument is interpreted as a comma separated list of the VRSetups to start
  /// and new processes are forked as needed so that we have one process per
  /// vrSetup.
  void initialize(const std::string &configFile, const std::string &vrSetups = "");


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

  void addInputDevice(VRInputDevice* dev);
 
  VRGraphicsToolkit* getGraphicsToolkit(const std::string &name);
  VRWindowToolkit* getWindowToolkit(const std::string &name);
   
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
  VRDisplayNode*                  _displayGraph;
};


} // end namespace

#endif
