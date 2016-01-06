#ifndef VRMAIN_H
#define VRMAIN_H

#include "config/VRDataIndex.h"
#include "net/VRNetClient.h"


/** Application programmers should use this singleton class as the
    interface to the MinVR library.
*/
class VRMain {
public:

  void registerEventCallback(void (*eventCB)
                                   (const std::string &,
                                    VRDataIndex *))
  { _eventCB = eventCB; };
  
  void registerRenderCallback(void (*renderCB)(VRDataIndex *))
  { _renderCB = renderCB; };

  void registerSwapCallback(void (*swapCB)()) { _swapCB = swapCB; };
                            
  
  // VRMain is a singleton class -- each program should contain exactly
  // one instance of VRMain.  Access the current instance using the
  // instance() function, which will create a new instance the first
  // time it is called, typically via:
  // VRMain::instance()->initialize(settings);
  static VRMain* instance();


  // THE BIG 4: TO USE MINVR, ADD THE FOLLOWING 4 FUNCTIONS TO YOUR PROGRAM:

  // 1. The first step is to initialize MinVR.  The settings file
  // is quite detailed and contains the information needed to setup
  // the geometry of the VR display devices, the type of stereo
  // rendering to use, input device drivers to create, plugins/add-ons
  // to load, and more.
  void initialize(const std::string settingsFile);


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

  virtual ~VRMain();

private:

  VRMain();
  static VRMain *_instance;

  bool initialized;

  VRNetClient   *_vrNet;
  //std::vector<VRInputDevice*> _inputDevices;
  //std::vector<VRDisplayDevice*> _displayDevices;

  VRDataIndex *_index;

  void (*_eventCB)(const std::string &, VRDataIndex *);
  void (*_renderCB)(VRDataIndex *);
  void (*_swapCB)();
};

#endif
