

#ifndef SHADOWS_H
#define SHADOWS_H

#include "../../VRG3D/include/CommonInc.H"


namespace MinVR {

PLUGIN_API void pushShadowState(G3D::RenderDevice *rd);
PLUGIN_API void popShadowState(G3D::RenderDevice *rd);

PLUGIN_API bool getShadowsOn();
PLUGIN_API void setShadowsOn(bool b);

} // end namespace

#endif
