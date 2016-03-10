/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRDISPLAYDEVICEFACTORY_H_
#define VRDISPLAYDEVICEFACTORY_H_

#include "VRDisplayDevice.h"
#include "config/VRDataIndex.h"

namespace MinVR {

class VRDisplayDeviceFactory {
public:
	virtual ~VRDisplayDeviceFactory() {}

	virtual VRDisplayDevice* create(VRDataIndex& config, const std::string nameSpace) = 0;
};

} /* namespace MinVR */

#endif /* VRDISPLAYGRAPHNODEFACTORY_H_ */
