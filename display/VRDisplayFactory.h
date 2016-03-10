/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRDISPLAYFACTORY_H_
#define VRDISPLAYFACTORY_H_

#include "VRDisplay.h"
#include "config/VRDataIndex.h"

namespace MinVR {

class VRDisplayFactory {
public:
	virtual ~VRDisplayFactory() {}

	virtual VRDisplay* create(VRDataIndex& config, const std::string nameSpace) = 0;
};

} /* namespace MinVR */

#endif /* VRDISPLAYFACTORY_H_ */
