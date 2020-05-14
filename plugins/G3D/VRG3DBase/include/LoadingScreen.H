/**
 * /author Dan Keefe (dfk)
 *
 * /file  LoadingScreen.H
 * /brief 
 *
 */ 

#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include "../../VRG3D/include/CommonInc.H"
#include "ConfigVal.H"
#include "GfxMgr.H"

namespace MinVR {

  PLUGIN_API class LoadingScreen
{
public:
  PLUGIN_API static void renderAndSwapBuffers(MinVR::GfxMgrRef gfxMgr, double percentDone);

  PLUGIN_API static void render(MinVR::GfxMgrRef gfxMgr, double percentDone);
};

} // end namespace

#endif

