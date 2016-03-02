#ifndef VRCALLBACKDISPLAYACTION_H_
#define VRCALLBACKDISPLAYACTION_H_

#include "VRDisplayAction.h"

namespace MinVR {

class VRCallbackDisplayAction : public VRDisplayAction {
public:
	typedef void (*MethodType)();

	VRCallbackDisplayAction(MethodType method);
	virtual ~VRCallbackDisplayAction();

	virtual void exec() const;

private:
	MethodType m_method;
};

}

#endif
