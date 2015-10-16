/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef DEVICEINTERFACE_H_
#define DEVICEINTERFACE_H_

#include "plugin/PluginInterface.h"

class InputDeviceFactory
{
public:
	virtual ~InputDeviceFactory() {}
	virtual bool createDevice(std::string type, std::string parameters) = 0;
};

class DeviceInterface : public MinVR::PluginInterface {
public:
	virtual void addInputDeviceFactory(InputDeviceFactory* factory) = 0;

	std::string getName() { return "DeviceInterface"; }
};

#endif /* DEVICEINTERFACE_H_ */

