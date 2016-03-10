/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRDISPLAYNODE_H_
#define VRDISPLAYNODE_H_

#include "display/VRDisplayDevice.h"
#include <vector>

namespace MinVR {

class VRDisplayNode : public VRDisplayDevice {
public:
	VRDisplayNode();
	virtual ~VRDisplayNode();

	virtual void render(VRRenderer& renderer);

	virtual const std::vector<VRDisplayDevice*>& getChildren() const;

protected:
	void addChild(VRDisplayDevice* child);

private:
	std::vector<VRDisplayDevice*> m_children;
};

} /* namespace MinVR */

#endif /* VRDISPLAYNODE_H_ */