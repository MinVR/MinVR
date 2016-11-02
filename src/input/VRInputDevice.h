#ifndef VRINPUTDEVICE_H
#define VRINPUTDEVICE_H

#include <vector>
#include <config/VRDataQueue.h>

/** Abstract base class for Input Devices.  Input devices are polled once per frame by
	VRMain and should return any new events generated since the last call.
 */

namespace MinVR {

class VRInputDevice {
public:
	virtual ~VRInputDevice() {}
	virtual void appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents) = 0;

	static std::string getAttributeName(){ return "inputdeviceType"; };
};

} // ending namespace MinVR
 
#endif
