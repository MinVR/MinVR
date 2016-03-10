#ifndef VRDISPLAY_H_
#define VRDISPLAY_H_

#include "VRRenderer.h"

namespace MinVR {

class VRDisplay
{
public:
	virtual ~VRDisplay() {}

	virtual void render(VRRenderer& renderer) = 0;
};

}

#endif
