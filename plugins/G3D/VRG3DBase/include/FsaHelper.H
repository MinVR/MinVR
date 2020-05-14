

#ifndef FSAHELPER_H
#define FSAHELPER_H

#include "../../VRG3D/include/CommonInc.H"
#include "../../VRG3D/include/Event.h"

namespace MinVR {


// Functors allow for callback functions that are class methods
// in C++.  Google "C++ functors" for more information.
class EventCallbackFunctor
{
public:
  EventCallbackFunctor() {}
  virtual ~EventCallbackFunctor() {}
  virtual void exec(MinVR::EventRef event) = 0;
};

template <class T>
class EventCallbackSpecificFunctor : public EventCallbackFunctor
{
public:
  typedef void (T::*MethodType)(MinVR::EventRef event);

  EventCallbackSpecificFunctor(T *obj, MethodType meth) {
    _obj = obj;
    _method = meth;
  }

  virtual ~EventCallbackSpecificFunctor() {}

  void exec(MinVR::EventRef event) {
    (_obj->*_method)(event);
  }
  
protected:
  T          *_obj;
  MethodType  _method;
};
// End functor bullshit



/// FsaArc class is only for use internally within the Fsa class
typedef G3D::ReferenceCountedPointer<class FsaArc> FsaArcRef;
class FsaArc : public G3D::ReferenceCountedObject
{
public:
  FsaArc(const std::string &arcName, 
	 const int &toState, 
	 const G3D::Array<std::string> &triggerEvents);
  FsaArc();  
  virtual ~FsaArc();

  void addCallback(EventCallbackFunctor *f);
  void callCallbacks(MinVR::EventRef event);

  std::string getName()         const { return _name; }
  int         getTo()           const { return _to; }
  int         getNumTriggers()  const { return _triggers.size(); }
  std::string getTrigger(int i) const { return _triggers[i]; }

protected:
  G3D::Array<EventCallbackFunctor*> _callbacks;
  G3D::Array<std::string>           _triggers;
  std::string                  _name;
  int                          _to;
};


/// FsaState class is only for use internally within the Fsa class
class FsaState
{
public:
  FsaState(const std::string &stateName);  
  FsaState();  
  virtual ~FsaState();
  
  PLUGIN_API void addArcRef(FsaArcRef a);

  PLUGIN_API void addEnterCallback(EventCallbackFunctor *f);
  PLUGIN_API void addExitCallback(EventCallbackFunctor *f);
  
  PLUGIN_API void callEnterCallbacks();
  PLUGIN_API void callExitCallbacks();

  FsaArcRef   getArcRef(int i)       { return _arcs[i]; }
  int         getNumArcs()     const { return _arcs.size(); }
  std::string getName()        const { return _name; }

protected:
  std::string      _name;
  G3D::Array<FsaArcRef> _arcs;
  G3D::Array<EventCallbackFunctor*>  _enterCallbacks;
  G3D::Array<EventCallbackFunctor*>  _exitCallbacks;
};


} // end namespace

#endif

