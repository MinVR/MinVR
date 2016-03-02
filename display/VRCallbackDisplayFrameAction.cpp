#include "VRCallbackDisplayFrameAction.h"

using namespace MinVR;

VRCallbackDisplayFrameAction::VRCallbackDisplayFrameAction(MethodType method) :
	m_method(method)
{

}

VRCallbackDisplayFrameAction::~VRCallbackDisplayFrameAction()
{

}

bool VRCallbackDisplayFrameAction::exec()
{
	return (*m_method)();
}
