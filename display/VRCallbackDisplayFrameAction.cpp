#include "VRCallbackDisplayFrameAction.h"

using namespace MinVR;

VRCallbackDisplayFrameAction::VRCallbackDisplayFrameAction(MethodType method) :
	method(method)
{

}

VRCallbackDisplayFrameAction::~VRCallbackDisplayFrameAction()
{

}

bool VRCallbackDisplayFrameAction::exec()
{
	return (*method)();
}
