#ifndef VRENGINE_H
#define VRENGINE_H

#include <map>
#include "GraphicsEngine.h"
#include "DisplayDevice.h"
#include "InputDevice.h"
#include "VRApp.h"


/** VREngine
 */
class VREngine {
public:

  /// Use the static create factory method rather than the constructor.  create() will return
  /// NULL if there was an error creating the VREngine (e.g., pluginDir not found).
  static VREngine* create(const std::string &pluginDir, const std::map<std::string,std::string> &configMap, GraphicsEngine *gfxEngine);
  virtual ~VREngine();
  

  /// Option 1:  This is the preferred method for running your application.  Just call run() and VREngine will
  /// start up your application, repeatedly call loopOnce() and then shutdown() when the application exits.
  void run(VRApp *app);

  /// Option 2:  If your program requires control of the mainloop, then you can replicate the functionality of
  /// VREngine::run() yourself quite easily.  Just create your own mainloop, and, inside it, call loopOnce() 
  /// repeatedly until it returns false, then call shutdown.  Everything VREngine needs to do once per frame is 
  /// contained inside this function.  It returns true during normal operation and false as a flag to indicate 
  /// the app is ready to shutdown.
  bool loopOnce(VRApp *app);

  /// Frees memory and closes network connections
  void shutdown();


  void addDisplayDevice(DisplayDevice *dd) { _displayDevices.push_back(dd); }
  void addInputDevice(InputDevice *id) { _inputDevices.push_back(id); }


private:
  VREngine(GraphicsEngine *gfxEngine);

  std::vector<DisplayDevice*> _displayDevices;
  std::vector<InputDevice*> _inputDevices;
  GraphicsEngine *_gfxEngine;

};

#endif
