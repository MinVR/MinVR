#ifndef VRDISPLAYDEVICE_H_
#define VRDISPLAYDEVICE_H_

namespace MinVR {

class VRDisplayDevice
{
public:
	virtual ~VRDisplayDevice() {}

	virtual void render() = 0;
};

}

#endif
