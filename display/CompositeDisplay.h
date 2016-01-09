/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef COMPOSITEDISPLAY_H_
#define COMPOSITEDISPLAY_H_

/*#include "display/VRDisplayDevice.h"

namespace MinVR {

class CompositeDisplayFactory : public VRDisplayDeviceFactory {
public:
	CompositeDisplayFactory(std::vector<VRDisplayDeviceFactory*> factories) : factories(factories) {}
	virtual ~CompositeDisplayFactory() {}

	std::vector<VRDisplayDevice*> create(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory)
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

private:
	std::vector<VRDisplayDeviceFactory*> factories;
};

class CompositeDisplay : public VRDisplayDevice {
public:
	CompositeDisplay(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory)
	{
		std::vector<VRDisplayDevice*> displays = factory->create(config, nameSpace, factory);
		for (int f = 0; f < displays.size(); f++)
		{
			addSubDisplay(displays[f]);
		}
	}

	virtual ~CompositeDisplay() {}

	void finishRendering()
	{
		finishRenderingAllDisplays();
	}

protected:
	void startRendering(const MinVR::VRRenderer& renderer, int x)
	{
		startRenderingAllDisplays(renderer, x);
	}
};



} /* namespace MinVR */

//#endif /* COMPOSITEDISPLAY_H_ */
