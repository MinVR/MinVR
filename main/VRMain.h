#ifndef VRCORE_H
#define VRCORE_H

#include "VRSettings.h"
#include "VREvent.h"
#include "VRRenderState.h"
#include "VRNetInterface.h"


/** Application programmers should use this singleton class as the interface to the MinVR library.
*/
class VRCore {
public:

  // VRCore is a singleton class -- each program should contain exacly one instance of VRCore.  Access the
  // current instance using the instance() function, which will create a new instance the first time it is
  // called, typically via:  VRCore::instance()->initialize(settings);
  static VRCore* instance();


  // THE BIG 4: TO USE MINVR, ADD THE FOLLOWING 4 FUNCTIONS TO YOUR PROGRAM:

  // 1. The first step is to initialize MinVR.  The settings parameter is quite detailed and contains the
  // information needed to setup the geometry of the VR display devices, the type of stereo rendering to 
  // use, input device drivers to create, plugins/add-ons to load, and more.
  void initialize(const VRSettings &settings);


  // BEGIN LOOP: YOUR PROGRAM SHOULD CONTAIN A MAIN LOOP THAT CALLS THE FOLLOWING TWO FUNCTIONS ONCE PER FRAME:

    // 2. Typically the first call in your program's mainloop.  First, MinVR syncs events across all vr nodes.
    // Then, MinVR processes them to update its own state (e.g., for headtracking).  Finally, MinVR calls your
    // event callback function so that your program can also respond to input events.
    void synchronizeAndProcessEvents(void (*eventCallbackFunction)(const std::vector<VREvent> &));


    // 3. Tell MinVR to apply the appropriate projection matrices, shaders, etc. for each DisplayDevice
    // setup during initilization and then call your draw function to actually render the scene via
    // OpenGL or whatever graphics engine you are using.
    void renderOnAllDisplayDevices(void (*renderCallbackFunction)(const VRRenderState &));

  // END LOOP


  // 4. Call shutdown() to close any network connections and/or other resources created by MinVR
  void shutdown();

  virtual ~VRCore();

private:

  VRCore();
  VRCore *_instance;

  VRDisplayManager *_displayManager;
  VRNetInterface   *_vrNet;
  std::vector<VRInputDevice*> _inputDevices;

};

#endif