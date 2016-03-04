/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRGRAPHICSWINDOWCHILD_H_
#define VRGRAPHICSWINDOWCHILD_H_

#include "display/base/VRDisplayNode.h"

namespace MinVR {

class VRGraphicsWindowChild : public VRDisplayNode {
public:
	VRGraphicsWindowChild();
	virtual ~VRGraphicsWindowChild();
};

} /* namespace MinVR */

#endif /* VRGRAPHICSWINDOWCHILD_H_ */
