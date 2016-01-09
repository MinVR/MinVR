/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/concrete/CompositeDisplayFactory.h>

namespace MinVR {

CompositeDisplayFactory::CompositeDisplayFactory(std::vector<VRDisplayDeviceFactory*> factories) : factories(factories) {
	// TODO Auto-generated constructor stub

}

CompositeDisplayFactory::~CompositeDisplayFactory() {
	// TODO Auto-generated destructor stub
}

std::vector<VRDisplayDevice*> CompositeDisplayFactory::create(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory)
{
	std::vector<VRDisplayDevice*> displays;

	for (int f = 0; f < factories.size(); f++)
	{
		std::vector<VRDisplayDevice*> factoryDisplays = factories[f]->create(config, nameSpace, factory);
		for (int i = 0; i < factoryDisplays.size(); i++)
		{
			displays.push_back(factoryDisplays[i]);
		}
	}

	return displays;
}

} /* namespace MinVR */
