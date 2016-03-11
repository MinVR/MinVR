/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRVIEWPORTNODE_H_
#define VRVIEWPORTNODE_H_

#include "display/graphics/VRGraphicsWindowChild.h"
#include "VRViewport.h"
#include "VRViewportFormatter.h"

namespace MinVR {

class VRViewportNode : public VRGraphicsWindowChild {
public:
	VRViewportNode();
	virtual ~VRViewportNode();

	void render(VRRenderer& renderer);

	virtual void addChild(VRGraphicsWindowChild* child);

private:
	VRViewportFormatter m_viewportFormatter;
	VRViewport m_viewport;
};

} /* namespace MinVR */

#endif /* VRVIEWPORTNODE_H_ */
