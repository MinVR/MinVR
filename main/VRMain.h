#ifndef VRMAIN_H
#define VRMAIN_H

#include "net/VRNetInterface.h"
#include "config/VRDataIndex.h"
#include "main/VRPluginInterface.h"
#include "plugin/PluginManager.h"
#include "display/VRDisplayGraphNode.h"

/** Application programmers should use this singleton class as the
    interface to the MinVR library.

    This version has been simplified a little (ok, well, a lot) in
    order to get it functioning.  That is, the list of input and
    display devices was put aside and needs to be re-implemented.

*/
class VRMain : public MinVR::VRPluginInterface {
public:

  VRMain();
  virtual ~VRMain();

  // Use these methods to register the various callbacks for the
  // execution.  Likely enough we'll have a longer list of these
  // eventually, and maybe a better way to organize them.
  void registerEventCallback(void (*eventCB)
                                   (const std::string &,
                                    VRDataIndex *))
  { _eventCB = eventCB; };
  
  void registerRenderCallback(void (*renderCB)(VRDataIndex *))
  { _renderCB = renderCB; };

  void registerSwapCallback(void (*swapCB)()) { _swapCB = swapCB; };
                            
  
  // THE BIG 4: TO USE MINVR, ADD THE FOLLOWING 4 FUNCTIONS TO YOUR PROGRAM:

  // 1. The first step is to initialize MinVR.  This is a separate
  // step from the constructor because it is frequently convenient for
  // your VRMain to be a global variable, and C++ does not allow a
  // convenient and reliable way to initialize global variables with
  // arguments.
  //  
  // The settings file is quite detailed and contains the information
  // needed to setup the geometry of the VR display devices, the type
  // of stereo rendering to use, input device drivers to create,
  // plugins/add-ons to load, and more.  The process name is used to
  // find the data that belongs to this process in the configuration
  // files.  Leave it out if such things are irrelevant to your
  // project.
  void initialize(const std::string settingsFile);
  void initialize(const std::string processName, const std::string settingsFile);
  void initialize(VRDataIndex& index, const std::string valName);
  void initialize(const std::string processName, VRDataIndex& index, const std::string valName);


  // BEGIN LOOP: YOUR PROGRAM SHOULD CONTAIN A MAIN LOOP THAT CALLS
  // THE FOLLOWING TWO FUNCTIONS ONCE PER FRAME:

  // 2. Typically this is the first call in your program's mainloop.
  // First, MinVR syncs events across all VR nodes.  Then, MinVR
  // processes them to update its own state (e.g., for headtracking).
  // Finally, MinVR calls your event callback function so that your
  // program can also respond to input events.
  void synchronizeAndProcessEvents();


  // 3. Tell MinVR to apply the appropriate projection matrices,
  // shaders, etc. for each DisplayDevice setup during initilization
  // and then call your draw function to actually render the scene via
  // OpenGL or whatever graphics engine you are using.
  virtual void renderEverywhere();

  // END LOOP


  // 4. Call shutdown() to close any network connections and/or other
  // resources created by MinVR
  void shutdown();

  std::string getProcessName() { return _name; }
  

  // Adds the display factories for all plugins who use this interface
  void addVRDisplayDeviceFactory(MinVR::VRDisplayGraphNodeFactory* factory);
  	// Adds the input device factories for all plugins who use this interface
  void addVRInputDeviceFactory(VRInputDeviceFactory* factory);
  	// Used for timing (i.e. for animation, etc...)
  void addVRTimer(MinVR::VRTimer* timer);

  MinVR::VRDisplayGraphNode* getDisplay() const {
		return _display;
	}

private:
	void initialize();


  std::string _name;
  // This flag indicates that the initialize() method has been run,
  // not simply that the object has been created.
  bool initialized;

  VRNetInterface   *_vrNet;
  // These have to be re-implemented.
  //std::vector<VRInputDevice*> _inputDevices;
  //std::vector<VRDisplayDevice*> _displayDevices;

  VRDataIndex *_index;

  void (*_eventCB)(const std::string &, VRDataIndex *);
  void (*_renderCB)(VRDataIndex *);
  void (*_swapCB)();

  MinVR::VRDisplayGraphNode* _display;

  // Plugin items
  MinVR::PluginManager _pluginManager;
  std::vector<MinVR::VRDisplayGraphNodeFactory*> _displayFactories;
  std::vector<VRInputDeviceFactory*> _inputDeviceFactories;
  std::vector<MinVR::VRTimer*> _timers;
  std::vector<VRInputDevice*> _inputDevices;
};

#endif
