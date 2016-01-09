/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/concrete/BaseDisplayFactory.h>

namespace MinVR {

BaseDisplayFactory::~BaseDisplayFactory() {
	for (int f = 0; f < displays.size(); f++)
	{
		delete displays[f];
	}
}

std::vector<VRDisplayDevice*> BaseDisplayFactory::create(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory)
{
	std::vector<VRDisplayDevice*> newDisplays;

	VRContainer item = config.getValue(nameSpace);

	for (VRContainer::iterator f = item.begin(); f != item.end(); f++)
	{
		if (config.getType(*f) == VRCORETYPE_CONTAINER) {

			VRContainer item = config.getValue(*f);
			if (config.exists("displayType", *f))
			{
				std::string displayType = config.getValue("displayType", *f);
				VRDisplayDevice* display = createDisplay(displayType, *f, config, factory);
				if (display != NULL)
				{
					newDisplays.push_back(display);
					displays.push_back(display);

					std::vector<VRDisplayDevice*> subDisplays = factory->create(config, *f, factory);
					for (int f = 0; f < subDisplays.size(); f++)
					{
						subDisplays[f]->setParent(display);
						display->addSubDisplay(subDisplays[f]);
					}
				}
			}
		}
	}

	return newDisplays;
}

} /* namespace MinVR */
