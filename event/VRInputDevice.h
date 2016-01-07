#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <vector>
#include "VREvent.h"
#include "data/VRDataIndex.h"
#include "plugin/PluginInterface.h"

/** InputDevice:
    An as-simple-as-possible public interface for Input Devices
 */
class VRInputDevice {
public:
	virtual ~VRInputDevice() {}
	virtual void appendNewInputEventsSinceLastCall(std::vector<VREvent> &inputEvents) = 0;
};

class VRInputDeviceDriver {
public:
	virtual ~VRInputDeviceDriver() {}

	VRInputDevice* create(const std::string &type, const std::string& name)
	{
		static VRDataIndex di;
		return create(type, name, di);
	}
  virtual VRInputDevice* create(const std::string &type,
                                const std::string& name,
                                VRDataIndex &config) = 0;
};

class VRInputDeviceInterface : public MinVR::PluginInterface {
public:
	virtual void addInputDeviceDriver(VRInputDeviceDriver* driver) = 0;

	std::string getName() { return "VRInputDeviceInterface"; }
	const std::type_info& getType() { return typeid(VRInputDeviceInterface); }
	int getMinVersion() { return getVersion(); }
	static int getVersion() { return 0; }
};

#endif
