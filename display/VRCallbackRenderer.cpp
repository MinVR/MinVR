#include "VRCallbackRenderer.h"

using namespace MinVR;

VRCallbackRenderer::VRCallbackRenderer(MethodType method) :
	m_method(method)
{

}

VRCallbackRenderer::~VRCallbackRenderer()
{
	
}

void VRCallbackRenderer::render(VRRenderState& state) const
{
	(*m_method)(state);
}
