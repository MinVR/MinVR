/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/concrete/CompositeDisplay.h>

namespace MinVR {

CompositeDisplay::CompositeDisplay(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory)
{
	std::vector<VRDisplayDevice*> displays = factory->create(config, nameSpace, factory);
	for (int f = 0; f < displays.size(); f++)
	{
		addSubDisplay(displays[f]);
	}
}

CompositeDisplay::~CompositeDisplay() {
	// TODO Auto-generated destructor stub
}

void CompositeDisplay::finishRendering()
{
	finishRenderingAllDisplays();
}

void CompositeDisplay::startRendering(const MinVR::VRRenderer& renderer, VRRenderState& state)
{
	startRenderingAllDisplays(renderer, state);
}

} /* namespace MinVR */
