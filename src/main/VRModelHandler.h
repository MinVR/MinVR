#ifndef VRMODELHANDLER_H
#define VRMODELHANDLER_H


namespace MinVR {

/** This abstract interface class provides the hook / callback mechanism for
    application programmers to update their models of the world.  Application
    programmers should subclass from VRModelHandler and then register their
    class with MinVR using VRMain::registerModelHandler(..);
*/

class VRModelHandler {
public:
  /// Called from within VRMain::mainloop().  You can use the FrameStart
  /// heartbeat events to establish a time and update the model here before
  /// rendering.  Or use it for something else we haven't thought of yet.
  virtual void updateWorld() {};
};


} // end namespace

#endif
