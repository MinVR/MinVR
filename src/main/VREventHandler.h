#ifndef VREVENTHANDLER_H
#define VREVENTHANDLER_H

namespace MinVR {


/** This abstract interface class provides the hook / callback mechanism for
    application programmers to respond to MinVR events.  Application 
    programmers should subclass from VREventHandler and then register their 
    class with MinVR using VRMain::registerEventHandler(..);
*/
class VREventHandler {
public:
  /// Called from within VRMain::synchronizeAndProcessEvents() once for each
  /// event generated since the last call to synchronizeAndProcessEvents().
  virtual void onVREvent(const std::string &eventName, VRDataIndex *eventData) = 0;
};


} // end namespace

#endif
