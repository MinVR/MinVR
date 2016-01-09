/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef COMPOSITEDISPLAYFACTORY_H_
#define COMPOSITEDISPLAYFACTORY_H_

#include "display/VRDisplayDeviceFactory.h"

namespace MinVR {

class CompositeDisplayFactory : public VRDisplayDeviceFactory {
public:
	CompositeDisplayFactory(std::vector<VRDisplayDeviceFactory*> factories);
	virtual ~CompositeDisplayFactory();

	std::vector<VRDisplayDevice*> create(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory);

private:
	std::vector<VRDisplayDeviceFactory*> factories;
};

} /* namespace MinVR */

#endif /* COMPOSITEDISPLAYFACTORY_H_ */
