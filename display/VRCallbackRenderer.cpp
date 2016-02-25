#include "VRCallbackRenderer.h"

using namespace MinVR;

VRCallbackRenderer::VRCallbackRenderer(MethodType method) :
	method(method)
{

}

VRCallbackRenderer::~VRCallbackRenderer()
{
	
}

void VRCallbackRenderer::render(VRRenderState& state) const
{
	(*method)(state);
}