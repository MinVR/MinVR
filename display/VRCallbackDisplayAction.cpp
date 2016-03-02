#include "VRCallbackDisplayAction.h"

using namespace MinVR;

VRCallbackDisplayAction::VRCallbackDisplayAction(MethodType method) : 
	m_method(method)
{

}

VRCallbackDisplayAction::~VRCallbackDisplayAction()
{
	
}

void VRCallbackDisplayAction::exec() const
{
	(*m_method)();
}
