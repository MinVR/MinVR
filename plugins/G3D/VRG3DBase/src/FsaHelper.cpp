

#include "../include/FsaHelper.H"


namespace MinVR {

using namespace G3D;

FsaArc::FsaArc(const std::string &arcName, 
	       const int &toState, 
	       const Array<std::string> &triggerEvents)
{
  _name = arcName;
  _to = toState;
  _triggers = triggerEvents;
}

FsaArc::FsaArc()
{
  _to = 0;
  _name = "default_arc";
}

FsaArc::~FsaArc()
{
}

void
FsaArc::addCallback(EventCallbackFunctor *f)
{
  _callbacks.append(f);
}

void
FsaArc::callCallbacks(MinVR::EventRef event)
{
  for (int i=0;i<_callbacks.size();i++) {
    _callbacks[i]->exec(event);
  }
}


FsaState::FsaState(const std::string &stateName)
{
  _name = stateName;
}

FsaState::FsaState()
{
  _name = "default_state";
}

FsaState::~FsaState()
{
}

void
FsaState::addArcRef(FsaArcRef a)
{
  _arcs.append(a);
}

void
FsaState::addEnterCallback(EventCallbackFunctor *f)
{
  _enterCallbacks.append(f);
}

void
FsaState::addExitCallback(EventCallbackFunctor *f)
{
  _exitCallbacks.append(f);
}

void
FsaState::callEnterCallbacks()
{
  MinVR::EventRef e = new Event(_name + "_stateEnter");
  for (int i=0;i<_enterCallbacks.size();i++) {
    _enterCallbacks[i]->exec(e);
  }
}

void
FsaState::callExitCallbacks()
{
  MinVR::EventRef e = new Event(_name + "_stateExit");
  for (int i=0;i<_exitCallbacks.size();i++) {
    _exitCallbacks[i]->exec(e);
  }
}


} // end namespace
