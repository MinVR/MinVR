#ifndef VRDISPLAYDEVICE_H_
#define VRDISPLAYDEVICE_H_

#include "VRRenderer.h"

namespace MinVR {

class VRDisplayDevice
{
public:
	virtual ~VRDisplayDevice() {}

	virtual void render(VRRenderer& renderer) = 0;
};

}

#endif
