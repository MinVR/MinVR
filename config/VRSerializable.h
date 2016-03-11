/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRSERIALIZABLE_H_
#define VRSERIALIZABLE_H_

#include "VRDataIndex.h"

namespace MinVR {

class VRSerializable {
public:
	virtual ~VRSerializable() {}

	virtual void serialize(VRDataIndex& index, std::string name) const = 0;
	virtual void deserialize(VRDataIndex& index, std::string name) = 0;
};

} /* namespace MinVR */

#endif /* VRSERIALIZABLE_H_ */
