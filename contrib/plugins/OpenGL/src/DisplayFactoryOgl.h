/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef DISPLAYFACTORYOGL_H_
#define DISPLAYFACTORYOGL_H_

#include "display/VRDisplayDevice.h"

namespace MinVR {

class DisplayFactoryOgl : public SimpleVRDisplayFactory {
public:
	DisplayFactoryOgl();
	virtual ~DisplayFactoryOgl();

	VRDisplayDevice* createDisplay(const std::string type, const std::string name, VRDataIndex& config, VRDisplayDeviceFactory* factory);
};

} /* namespace MinVR */

#endif /* DISPLAYFACTORYOGL_H_ */
