#ifndef VRMODELHANDLER_H
#define VRMODELHANDLER_H


namespace MinVR {

/** This abstract interface class provides the hook / callback mechanism for
    application programmers to update their models of the world.  Application
    programmers should subclass from VRModelHandler and then register their
    class with MinVR using VRMain::registerModelHandler(..);
*/

/// \brief Takes care of the simulation model.
///
/// The model handler class takes care of the simulation your graphics
/// program is rendering, but in a way that is independent of the event
/// handler and render loop.  This can be important on systems with
/// multiple displays, where even the graphics context handler can be
/// called multiple times in a single iteration of the main render loop.
class VRModelHandler {
public:
  /// \brief Update the simulation to the current time.
  ///
  /// Called from within VRMain::mainloop().  Use a model handler to update
  /// the model to be viewed.  The input is just the current time, as
  /// understood by MinVR.
  virtual void updateWorld(double currentTime) {};
};


} // end namespace

#endif
