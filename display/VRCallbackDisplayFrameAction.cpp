#include "VRCallbackDisplayFrameAction.h"

using namespace MinVR;

VRCallbackDisplayFrameAction::VRCallbackDisplayFrameAction(MethodType method) :
	method(method)
{

}

bool VRCallbackDisplayFrameAction::exec()
{
	return (*method)();
}