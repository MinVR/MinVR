/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRDISPLAYDEVICEFACTORY_H_
#define VRDISPLAYDEVICEFACTORY_H_

#include <vector>
#include "display/VRDisplayDevice.h"

namespace MinVR {

class VRDisplayDeviceFactory {
public:
	virtual ~VRDisplayDeviceFactory() {}

	virtual std::vector<VRDisplayDevice*> create(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory) = 0;
};

} /* namespace MinVR */

#endif /* VRDISPLAYDEVICEFACTORY_H_ */
