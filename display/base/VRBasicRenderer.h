/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRBASICRENDERER_H_
#define VRBASICRENDERER_H_

#include "display/VRRenderer.h"
#include "VRRenderCallback.h"
#include "VRFunctionRenderCallback.h"

namespace MinVR {

class VRBasicRenderer : public VRRenderer {
public:
	VRBasicRenderer(VRRenderCallback& renderCallback);
	virtual ~VRBasicRenderer();

	void updateFrame();
	void render();

	VRRenderState& getState();
	void pushState();
	void popState();
	void resetState();

private:
	VRRenderCallback* m_renderCallback;
	VRRenderState m_state;
	int m_nodeNum;
	std::vector<std::string> m_nameSpaces;
};

} /* namespace MinVR */

#endif /* VRBASICRENDERER_H_ */
