#ifndef VRCALLBACKDISPLAYFRAMEACTION_H_
#define VRCALLBACKDISPLAYFRAMEACTION_H_

#include "display/VRDisplayFrameAction.h"

namespace MinVR {

class VRCallbackDisplayFrameAction : public VRDisplayFrameAction {
public:
	typedef bool (*MethodType)();

	VRCallbackDisplayFrameAction(MethodType method);
	virtual ~VRCallbackDisplayFrameAction();

	bool exec();

private:
	MethodType method;
};

}

#endif
