/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <DisplayFactoryOgl.h>
#include "VRViewportOgl.h"
#include "stereo/SideBySideStereo.h"
#include "stereo/QuadbufferStereo.h"
#include <iostream>

namespace MinVR {

DisplayFactoryOgl::DisplayFactoryOgl() {
	// TODO Auto-generated constructor stub

}

DisplayFactoryOgl::~DisplayFactoryOgl() {
	// TODO Auto-generated destructor stub
}

VRDisplayDevice* DisplayFactoryOgl::createDisplay(const std::string type,
		const std::string name, VRDataIndex& config,
		VRDisplayDeviceFactory* factory) {
	if (type == "viewport")
	{
		int xOffset = config.getValue("xOffset", name);
		int yOffset = config.getValue("yOffset", name);
		int width = config.getValue("width", name);
		int height = config.getValue("height", name);

		return new VRViewportOgl(xOffset,yOffset,width,height);
	}
	else if (type == "sideBySideStereo")
	{
		return new SideBySideStereo();
	}
	else if (type == "quadbufferStereo")
	{
		return new QuadbufferStereo();
	}

	return NULL;
}

} /* namespace MinVR */
