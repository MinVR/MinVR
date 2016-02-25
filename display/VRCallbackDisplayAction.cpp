#include "VRCallbackDisplayAction.h"

using namespace MinVR;

VRCallbackDisplayAction::VRCallbackDisplayAction(MethodType method) : 
	method(method)
{

}

VRCallbackDisplayAction::~VRCallbackDisplayAction()
{
	
}

void VRCallbackDisplayAction::exec() const
{
	(*method)();
}