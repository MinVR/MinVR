/**
 * \author Daniel Keefe (dfk)
 *
 * \file  Fsa.H
 * \brief
 *
 */

#ifndef FSA_H
#define FSA_H

//#include <SDL.h>
#include "../../VRG3D/include/CommonInc.H"



// include Event specifications and functors to allow class method callbacks
#include "../../VRG3D/include/Event.h"
// include FsaArc and FsaState helper classes
#include "../include/FsaHelper.H"

namespace MinVR {

typedef G3D::ReferenceCountedPointer<class Fsa> FsaRef;

/** @breif a class for Fsa's used to track input events
 *
 *
 * These are particularly useful for scenarios where a state is entered after a
 * click for gestures, and then input has to be handled in a much different
 * manner than before the click.
 */
class Fsa : public G3D::ReferenceCountedObject
{
public:

  /** Reads in a .fsm file created by the qfsm FSA drawing program
      freely available on sourceforge.  Assumes that arc names are
      specified in the Input field of each Transition arc and that
      trigger events are in a space separated list entered into the
      arc's description field.  State names are extracted from the
      state name field as you would expect.
  */
  static FsaRef fromQFSMFile(const std::string &filename);

  /// Constructs a new Fsa object with no states or arcs.  Use the addState
  /// and addArc methods to design the Fsa.
  PLUGIN_API Fsa(const std::string &name);
  PLUGIN_API virtual ~Fsa();

  /// Adds a state to the fsa
  PLUGIN_API bool addState(const std::string &stateName);

  /** Adds an arc connecting two states together to the fsa.  If fromState
      and toState are the same, then no StateEnter/StateExit callbacks will
      be called when the arc is triggered and the Fsa's currentState will
      remain unchanged.  Triggers is an array of event names that should
      trigger a transition on this arc.
  */
  PLUGIN_API bool addArc(const std::string &arcName,
	      const int &fromState,
	      const int &toState,
	      const G3D::Array<std::string> &triggers);

  /** Adds an arc connecting two states together to the fsa.  If fromState
      and toState are the same, then no StateEnter/StateExit callbacks will
      be called when the arc is triggered and the Fsa's currentState will
      remain unchanged.  Triggers is an array of event names that should
      trigger a transition on this arc.
  */
  PLUGIN_API bool addArc(const std::string &arcName,
	      const std::string &fromState,
	      const std::string &toState,
	      const G3D::Array<std::string> &triggers);

  /** Adds arcs connecting any of fromStates to toState in the fsa.  If fromState
      and toState are the same, then no StateEnter/StateExit callbacks will
      be called when the arc is triggered and the Fsa's currentState will
      remain unchanged.  Triggers is an array of event names that should
      trigger a transition on this arc.
  */
  PLUGIN_API bool addArcs(const std::string &arcName,
	      const G3D::Array<int> &fromStates,
	      const int &toState,
	      const G3D::Array<std::string> &triggers){
    bool res=true;

    for(int i=0; i < fromStates.size(); i++){
      res = res && addArc(arcName, fromStates[i], toState, triggers);
    }
    return res;
  }

  /** Adds arcs connecting any of fromStates to toState in the fsa.  If fromState
      and toState are the same, then no StateEnter/StateExit callbacks will
      be called when the arc is triggered and the Fsa's currentState will
      remain unchanged.  Triggers is an array of event names that should
      trigger a transition on this arc.
  */ 
  PLUGIN_API bool addArcs(const std::string &arcName,
	      const G3D::Array<std::string> &fromStates,
	      const std::string &toState,
	      const G3D::Array<std::string> &triggers);


  /** Specify a method to call whenever the state stateName is entered
      as a result of a transition along an arc in the Fsa.  thisPtr
      must be a pointer to a class and method must be a method of that
      class that takes a single argument of type VRG3D::EventRef.  Callbacks
      are called in this order: StateExit, Arc, then StateEnter.
  */
  template <class T>
  void addStateEnterCallback(std::string stateName, T *thisPtr, 
			     void (T::*method)(MinVR::EventRef)) {
    EventCallbackFunctor *f = 
      new EventCallbackSpecificFunctor<T>(thisPtr, method);
    _states[getIndexOfState(stateName)].addEnterCallback(f);
  }


  /** Specify a method to call whenever the state stateName is exited
      as a result of a transition along an arc in the Fsa.  thisPtr
      must be a pointer to a class and method must be a method of that
      class that takes a single argument of type VRG3D::EventRef.  Callbacks
      are called in this order: StateExit, Arc, then StateEnter.
  */
  template <class T>
  void addStateExitCallback(std::string stateName, T *thisPtr, 
			    void (T::*method)(MinVR::EventRef)) {
    EventCallbackFunctor *f = 
      new EventCallbackSpecificFunctor<T>(thisPtr, method);
    _states[getIndexOfState(stateName)].addExitCallback(f);
  }
  

  /** Specify a method to call whenever the arc is traversed in the
      Fsa.  thisPtr must be a pointer to a class and method must be a
      method of that class that takes a single argument of type
      VRG3D::EventRef.  Callbacks are called in this order: StateExit, Arc,
      then StateEnter.
  */
  template <class T>
  void addArcCallback(std::string arcName, T *thisPtr, 
		      void (T::*method)(MinVR::EventRef)) {
    EventCallbackFunctor *f = 
      new EventCallbackSpecificFunctor<T>(thisPtr, method);
    storeArcCallback(arcName, f);
  }



  void print();

  void setDebug(bool b)  { _debug = b; }

  std::string getCurrentState() { return _states[_curState].getName(); }

  void processEvent(MinVR::EventRef event);

  /** Use this with care, this causes the Fsa to jump directly to the
      named state.  If the state belongs to this Fsa, the jump works
      and we return true, otherwise false is returned.  This overrides
      the normal FSA behavior!!!  Since you're not following one of
      the FSA's Arcs, no Arc callbacks are made.  If toState is
      different than the FSA's current state then StateExit and
      StateEnter callbacks are called. 
  */
  PLUGIN_API bool jumpToState(const std::string &toState);

  PLUGIN_API std::string getName() { return _name; }

protected:  

  std::string stateName(int i);   

  int getIndexOfState(std::string stateName) {
    for (int s=0;s<_states.size();s++) {
      if (_states[s].getName() == stateName)
        return s;
    }
    alwaysAssertM(false, "Can't find match for state name " + stateName);
    return -1;
  }


  PLUGIN_API bool checkTriggerMatch(MinVR::EventRef event, const std::string &triggerName);
  
  PLUGIN_API void storeArcCallback(std::string arcName, EventCallbackFunctor *f);
  
  G3D::Array<FsaState>  _states;
  std::string      _name;
  int              _curState;
  bool             _debug;
};


} // end namespace

#endif
