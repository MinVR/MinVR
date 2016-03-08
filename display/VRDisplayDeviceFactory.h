/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRDISPLAYGRAPHNODEFACTORY_H_
#define VRDISPLAYGRAPHNODEFACTORY_H_

#include "VRDisplayGraphNode.h"
#include "config/VRDataIndex.h"

namespace MinVR {

class VRDisplayGraphNodeFactory {
public:
	virtual ~VRDisplayGraphNodeFactory() {}

	virtual VRDisplayGraphNode* create(VRDataIndex& config, const std::string nameSpace, VRDisplayGraphNodeFactory* factory) = 0;
};

} /* namespace MinVR */

#endif /* VRDISPLAYGRAPHNODEFACTORY_H_ */
