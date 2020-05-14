
#ifndef EVENTFILTER_H
#define EVENTFILTER_H

//#include "CommonInc.H"
#include "../../VRG3D/include/Event.h"

namespace MinVR {


typedef G3D::ReferenceCountedPointer<class EventFilter> EventFilterRef;
class EventFilter : public G3D::ReferenceCountedObject
{
public:
  EventFilter() {}
  virtual ~EventFilter() {}

  /// Subclasses implement this method, when a filter is added to FsaMgr
  /// its filter method is called for every event that gets queued.  The
  /// filter can do whatever it wants with that event, usually it will 
  /// generate different events based on its data.  If the filter method
  /// returns true then the original event gets queued as usual, if it
  /// returns false, then the original event is not queued, and therefore
  /// never processed by Fsa's.
  virtual bool filter(MinVR::EventRef e) = 0;
};
  


} // end namespace

#endif

