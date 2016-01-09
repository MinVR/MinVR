/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef BASEDISPLAYFACTORY_H_
#define BASEDISPLAYFACTORY_H_

#include "display/VRDisplayDeviceFactory.h"

namespace MinVR {

class BaseDisplayFactory : public VRDisplayDeviceFactory {
public:
	virtual ~BaseDisplayFactory();

	std::vector<VRDisplayDevice*> create(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory);
	virtual VRDisplayDevice* createDisplay(const std::string type, const std::string name, VRDataIndex& config, VRDisplayDeviceFactory* factory) = 0;

protected:
	std::vector<VRDisplayDevice*> displays;
};

} /* namespace MinVR */

#endif /* BASEDISPLAYFACTORY_H_ */
