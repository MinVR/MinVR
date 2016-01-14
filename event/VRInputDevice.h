#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <vector>
#include "config/VRDataIndex.h"
#include "config/VRDataQueue.h"

/** InputDevice:
    An as-simple-as-possible public interface for Input Devices
 */
class VRInputDevice {
public:
	virtual ~VRInputDevice() {}
	virtual void appendNewInputEventsSinceLastCall(VRDataQueue& queue) = 0;
};

class VRInputDeviceFactory {
public:
	virtual ~VRInputDeviceFactory() {}

    virtual std::vector<VRInputDevice*> create(VRDataIndex& dataIndex) = 0;
};

#endif
