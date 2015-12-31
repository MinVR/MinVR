#ifndef DISPLAYDEVICE_H
#define DISPLAYDEVICE_H

#include "config/VRDataIndex.h"
#include "display/VRRenderer.h"
#include <vector>

/** DisplayDevice:
    An as-simple-as-possible public interface for Display Devices
 */
class VRDisplayDevice {
public:
	virtual ~VRDisplayDevice() {}

	virtual bool isOpen() = 0;
	virtual void use() = 0;
	virtual void release() = 0;
	virtual void startRendering(const MinVR::VRRenderer& renderer) = 0;
	virtual void finishRendering() = 0;
};

class VRDisplayDeviceFactory {
public:
	virtual ~VRDisplayDeviceFactory() {}

	virtual std::vector<VRDisplayDevice*> create(const VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory) = 0;
};


#endif
