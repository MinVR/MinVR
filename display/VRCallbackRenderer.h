#ifndef VRCALLBACKRENDERER_H_
#define VRCALLBACKRENDERER_H_

#include "display/VRRenderer.h"

namespace MinVR {

class VRCallbackRenderer : public VRRenderer {
public:
	typedef void (*MethodType)(VRRenderState&);

	VRCallbackRenderer(MethodType method);
	virtual ~VRCallbackRenderer();

	void render(VRRenderState& state) const;

private:
	MethodType m_method;
};

}

#endif
