

#include "../include/Fsa.H"
#include "../include/StringUtils.H"

//#include <iostream>

namespace MinVR {

using namespace G3D;

Fsa::Fsa(const std::string &name)
{
  _curState = 0;
  _name = name;
  _debug = false;
}

Fsa::~Fsa()
{
}

std::string
Fsa::stateName(int i)
{
  debugAssert(i < _states.size());
  return _states[i].getName();
}

bool
Fsa::addState(const std::string &stateName)
{
  _states.append(stateName);
  return true;
}

bool
Fsa::addArc(const std::string &arcName,
	    const int &fromState,
	    const int &toState,
	    const Array<std::string> &triggers) 
{
  debugAssert(fromState < _states.size());

  FsaArcRef arc = new FsaArc(arcName, toState, triggers);
  _states[fromState].addArcRef(arc);
  return true;
}

bool
Fsa::addArc(const std::string &arcName,
	    const std::string &fromState,
	    const std::string &toState,
	    const Array<std::string> &triggers) 
{
  int from = -1;
  int to = -1;
  for (int i=_states.size()-1;i>=0;i--) {
    if (_states[i].getName() == fromState)
      from = i;
    if (_states[i].getName() == toState)
      to = i;
  }
  debugAssert(from != -1);
  debugAssert(to != -1);
  return addArc(arcName, from, to, triggers);
}

bool
Fsa::addArcs(const std::string &arcName,
	    const Array<std::string> &fromState,
	    const std::string &toState,
	    const Array<std::string> &triggers) 
{
  int to = -1;
  Array<int> inds;
  for (int i=_states.size()-1;i>=0;i--) {
    if (_states[i].getName() == toState)
      to = i;
  }
  for(int j=0; j < fromState.size(); j++){
    int from = -1;
    for (int i=_states.size()-1;i>=0;i--) {
      if (_states[i].getName() == fromState[j])
        from = i;
    }
    debugAssert(from != -1);
    inds.append(from);
  }
  debugAssert(to != -1);
  return addArcs(arcName, inds, to, triggers);
}


FsaRef
Fsa::fromQFSMFile(const std::string &filename)
{
  std::string instr;

  if (FileSystem::exists(filename)) {
    instr = readWholeFile(filename);
  }
  else {
    alwaysAssertM(false, "Cannot locate FSA description file \"" + filename + "\"");
  }


  std::string input = convertNewlinesAndTabsToSpaces(instr);
  Table<std::string, std::string> propertiesAndValues;
  std::string fieldData;
  std::string leftover;

  // first grab the fsa's name from the name property of the machine field in
  // the XML file
  std::string fsaName;
  if (getXMLField(input, "machine", propertiesAndValues, fieldData, leftover)) {
    if (!propertiesAndValues.containsKey("name")) {
      alwaysAssertM(false, "No XML \"name\" property found for field \"machine\"");
    }
    fsaName = propertiesAndValues["name"];
    //std::cout << "Reading FSA \"" << fsaName << "\"" << std::endl;
  }
  else {
    alwaysAssertM(false, "No \"machine\" field found in the XML file");
  }
  
  Fsa *fsa = new Fsa(fsaName);

  // now, read each state field
  leftover = input;
  for (int i=0; getXMLField(leftover, "state", propertiesAndValues, 
			    fieldData, leftover); i++) {
    //std::cout << "  State #" << i << " \"" << values << "\"" << std::endl;
    fsa->addState(fieldData);
  }

  // now, read each transition field
  for (int i=0; getXMLField(leftover, "transition", propertiesAndValues, 
			    fieldData, leftover); i++) {
    if (!propertiesAndValues.containsKey("description")) {
      alwaysAssertM(false, "No description property found for FSA Arc input field");
    }
    Array<std::string> triggers = 
      splitStringIntoArray(propertiesAndValues["description"]);
    
    std::string fromStr;
    std::string toStr;
    std::string arcName;
    std::string dummy;
    getXMLField(fieldData, "from",   propertiesAndValues, fromStr, dummy);
    getXMLField(fieldData, "to",     propertiesAndValues, toStr,   dummy);
    getXMLField(fieldData, "inputs", propertiesAndValues, arcName, dummy);
    
    //std::cout << "  Arc #" << i << " \"" << arcName << "\"" << std::endl;
    //std::cout << "    " << fsa->stateName(from) << " -> " 
    //	      << fsa->stateName(to) << std::endl;
    //std::cout << "    " << "Triggered by these event(s):" << std::endl;
    //for (int i=0;i<triggers.size();i++) {
      //std::cout << "      " << triggers[i] << std::endl;
    //}

    fsa->addArc(arcName, stringToInt(fromStr), stringToInt(toStr), triggers);
  }

  return fsa;
}


void
Fsa::print()
{
  std::cout << "FSA: \"" << _name << "\"" << std::endl;
  std::cout << " States:" << std::endl;
  for (int i=0;i<_states.size();i++) {
    std::cout << "  #" << i <<  " \"" << stateName(i) << "\"";
    if (i == _curState)
      std::cout << "  *** ACTIVE ***" << std::endl;
    else
      std::cout << std::endl;
  }
  std::cout << " Arcs:" << std::endl;
  for (int s=0;s<_states.size();s++) {
    for (int a=0;a<_states[s].getNumArcs();a++) {
      std::cout << "  \"" << _states[s].getArcRef(a)->getName() << "\" : " 
		<< stateName(s) << " --> " 
		<< stateName(_states[s].getArcRef(a)->getTo()) << std::endl;
      std::cout << "   Triggered by event(s):" << std::endl;
      for (int t=0;t<_states[s].getArcRef(a)->getNumTriggers();t++) {
	std::cout << "     " << _states[s].getArcRef(a)->getTrigger(t) << std::endl;
      }
    }
  }
}



bool
Fsa::checkTriggerMatch(MinVR::EventRef event, const std::string &triggerName)
{
  if (event->getName() == triggerName)
    return true;
  else if (beginsWith(triggerName, "ALL")) {

    if (triggerName == "ALL")
      return true;
    else if ((triggerName == "ALL_STANDARD") && 
	     (event->getType() == Event::EVENTTYPE_STANDARD))
      return true;
    else if ((triggerName == "ALL_1D") && 
	     (event->getType() == Event::EVENTTYPE_1D))
      return true;
    else if ((triggerName == "ALL_2D") && 
	     (event->getType() == Event::EVENTTYPE_2D))
      return true;
    else if ((triggerName == "ALL_COORDINATEFRAME") && 
	     (event->getType() == Event::EVENTTYPE_COORDINATEFRAME))
      return true;

  }

  return false;
}

void
Fsa::processEvent(MinVR::EventRef event)
{
  // check all the triggers for all the arcs of the current state
  // if a trigger matches this event then, follow that arc and call
  // a callback function if one is specified.


  if (_debug) {
    std::cout << "> FSA=\"" << _name << "\"; Current state=\"" 
	      << _states[_curState].getName() << "\""
	      << "; Processing event \"" << event->getName() << "\"" << std::endl;
  }
  
  int a=0;
  bool match=false;
  while ((!match) && (a<_states[_curState].getNumArcs())) {

    int t=0;
    if (_debug) {
      std::cout << "  > Checking arc \"" 
		<< _states[_curState].getArcRef(a)->getName() 
		<< "\"" << std::endl;
    }

    while ((!match) && (t<_states[_curState].getArcRef(a)->getNumTriggers())) {

      if (_debug) {
	std::cout << "    > Checking trigger \"" 
		  << _states[_curState].getArcRef(a)->getTrigger(t)
		  << "\" " << std::endl;
      }

      match = checkTriggerMatch(event, _states[_curState].getArcRef(a)->getTrigger(t));

      if (match) {
	// got match, transition on this arc..
	match = true;
	int lastState = _curState;
	_curState = _states[lastState].getArcRef(a)->getTo();
	bool leavesState = (lastState != _curState);

	if (_debug) {
	  std::cout << "      *** MATCH ***" << std::endl;
	   if (leavesState)
	     std::cout << "      > Setting current state to \"" 
		       << _states[_curState].getName() << "\"" << std::endl;
	   else 
	     std::cout << "      > No state change for this arc." << std::endl;
	}
	
	if (leavesState) {
	  if (_debug)
	    std::cout << "      > Calling state exit callbacks.." << std::endl;
	  _states[lastState].callExitCallbacks();
	}

	if (_debug)
	  std::cout << "      > Calling arc transition callbacks.." << std::endl;
	_states[lastState].getArcRef(a)->callCallbacks(event);
	
	if (leavesState) {
	  if (_debug)
	    std::cout << "      > Calling state enter callbacks.." << std::endl;
	  _states[_curState].callEnterCallbacks();
	}
      }
      t++;
    }
    a++;
  }
}

void
Fsa::storeArcCallback(std::string arcName, EventCallbackFunctor *f) 
{
  bool match = false;
  for (int s=0;s<_states.size();s++) {
    for (int a=0;a<_states[s].getNumArcs();a++) {
      if (_states[s].getArcRef(a)->getName() == arcName) {
	match = true;
	_states[s].getArcRef(a)->addCallback(f);
      }
    }
  }
  alwaysAssertM(match, "Can't add callback, can't find arc " + arcName);
}


bool
Fsa::jumpToState(const std::string &toState)
{
  for (int i=0;i<_states.size();i++) {
    if (_states[i].getName() == toState) {
      if (i != _curState) {
	_states[_curState].callExitCallbacks();
	_states[i].callEnterCallbacks();	  
      }
      _curState = i;
      return true;
    }
  }
  return false;
}


} // end namespace
