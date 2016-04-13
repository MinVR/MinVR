/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef DISPLAYFACTORYOGL_H_
#define DISPLAYFACTORYOGL_H_

#include "plugin/PluginFramework.h"
#include "display/concrete/BaseDisplayFactory.h"

namespace MinVR {

class DisplayFactoryOgl : public BaseDisplayFactory {
public:
	PLUGIN_API DisplayFactoryOgl();
	PLUGIN_API virtual ~DisplayFactoryOgl();

	PLUGIN_API VRDisplayDevice* createDisplay(const std::string type, const std::string name, VRDataIndex& config, VRDisplayDeviceFactory* factory);
};

} /* namespace MinVR */

#endif /* DISPLAYFACTORYOGL_H_ */
