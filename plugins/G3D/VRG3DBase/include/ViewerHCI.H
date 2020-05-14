/**
 * \author Daniel Keefe (dfk)
 *
 * \file  ViewerHCI.H
 * \brief
 *
 */


#ifndef VIEWERHCI_H
#define VIEWERHCI_H

#include "../../VRG3D/include/CommonInc.H"
#include "EventMgr.H"
#include "Fsa.H"
#include "GfxMgr.H"

namespace MinVR {

/**
  This viewer assumes the model is centered around (0,0,0).

  Controls:

  Mouse with left btn - trackball rotate
  Arrow keys          - pan left, right, up, and down
  "I" and "O"         - dolly in and out
  PAGEUP and PAGEDOWN - scale model up/down
  SPACEBAR            - start/stop rocking the model back and forth
  "F" and "S"         - make rocking faster or slower
  "W" and "N"         - make rocking angle wider or narrower
*/
class ViewerHCI
{
public:  
  PLUGIN_API ViewerHCI(EventMgrRef eventMgr, GfxMgrRef gfxMgr);
  PLUGIN_API virtual ~ViewerHCI();

  PLUGIN_API void trackballOn(MinVR::EventRef e);
  PLUGIN_API void trackballMove(MinVR::EventRef e);
  PLUGIN_API void trackballOff(MinVR::EventRef e);

  PLUGIN_API void toggleRocking(MinVR::EventRef e);
  PLUGIN_API void speedUpRocking(MinVR::EventRef e);
  PLUGIN_API void slowDownRocking(MinVR::EventRef e);
  PLUGIN_API void incRockingAngle(MinVR::EventRef e);
  PLUGIN_API void decRockingAngle(MinVR::EventRef e);

  PLUGIN_API void panLeft(MinVR::EventRef e);
  PLUGIN_API void panRight(MinVR::EventRef e);
  PLUGIN_API void panUp(MinVR::EventRef e);
  PLUGIN_API void panDown(MinVR::EventRef e);
  PLUGIN_API void dollyIn(MinVR::EventRef e);
  PLUGIN_API void dollyOut(MinVR::EventRef e);
  PLUGIN_API void scaleUp(MinVR::EventRef e);
  PLUGIN_API void scaleDown(MinVR::EventRef e);

  PLUGIN_API void poseForAnimation(G3D::Array<G3D::SurfaceRef> &posedModels, const G3D::CoordinateFrame &virtualToRoomSpace);

  PLUGIN_API void setRotCenter(G3D::Vector3 v){
    _boundingSphere.center=v;
  }

protected:
  EventMgrRef     _eventMgr;
  FsaRef          _fsa;
  GfxMgrRef       _gfxMgr;
  double          _lastRotTime;
  double          _rotAngularVel;
  G3D::Sphere          _boundingSphere;
  G3D::Vector3         _lastIntersectionPt;
  int             _animationCallbackID;
  double          _panAmt;
  bool            _rocking;
  double          _rockingDir;
  double          _rockingAngle;
  double          _totalAngle;
};

}
#endif
