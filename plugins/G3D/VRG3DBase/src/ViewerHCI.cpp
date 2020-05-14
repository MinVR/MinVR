#include "../include/ViewerHCI.H"

#include "../include/ConfigVal.H"
#include "../include/StringUtils.H"
#include "../../VRG3D/include/SynchedSystem.h"

namespace MinVR {

using namespace G3D;

ViewerHCI::ViewerHCI(EventMgrRef eventMgr, GfxMgrRef gfxMgr)
{
  _eventMgr = eventMgr;
  _gfxMgr = gfxMgr;

  _rocking = false;
  _rockingDir = 1.0;
  _totalAngle = 0.0;
  _rotAngularVel = ConfigVal("ViewerHCI_RockingSpeed", 0.35, false);
  _rockingAngle = ConfigVal("ViewerHCI_RockingAngle", 0.8, false);

  _boundingSphere = Sphere(Vector3::zero(), ConfigVal("ViewerHCI_TrackballRad", 0.55, false));

  _panAmt = ConfigVal("ViewerHCI_PanIncrement", 0.05, false);

  _fsa = new Fsa("ViewerHCI FSA");
  _fsa->addState("Start");
  _fsa->addState("Trackball");

  _fsa->addArc("TrackOn", "Start", "Trackball",
       splitStringIntoArray(ConfigVal("ViewerHCI_TrackballOn", "Mouse_Left_Btn_down", false)));
  _fsa->addArcCallback("TrackOn", this, &ViewerHCI::trackballOn);

  _fsa->addArc("TrackMove", "Trackball", "Trackball",
               splitStringIntoArray("Mouse_Pointer"));
  _fsa->addArcCallback("TrackMove", this, &ViewerHCI::trackballMove);

  _fsa->addArc("TrackOff", "Trackball", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_TrackballOff", "Mouse_Left_Btn_up", false)));
  _fsa->addArcCallback("TrackOff", this, &ViewerHCI::trackballOff);
  

  _fsa->addArc("Rock", "Start", "Start", 
       splitStringIntoArray(ConfigVal("ViewerHCI_ToggleRocking", "kbd_SPACE_down", false)));
  _fsa->addArcCallback("Rock", this, &ViewerHCI::toggleRocking);

  _fsa->addArc("ScaleUp", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_ScaleUp", "kbd_PAGEUP_down", false)));
  _fsa->addArcCallback("ScaleUp", this, &ViewerHCI::scaleUp);
  _fsa->addArc("ScaleDown", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_ScaleDown", "kbd_PAGEDOWN_down", false)));
  _fsa->addArcCallback("ScaleDown", this, &ViewerHCI::scaleDown);

  _fsa->addArc("DollyIn", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_DollyIn", "kbd_I_down", false)));
  _fsa->addArcCallback("DollyIn", this, &ViewerHCI::dollyIn);
  _fsa->addArc("DollyOut", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_DollyOut", "kbd_O_down", false)));
  _fsa->addArcCallback("DollyOut", this, &ViewerHCI::dollyOut);

  _fsa->addArc("PanUp", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_PanUp", "kbd_UP_down", false)));
  _fsa->addArcCallback("PanUp", this, &ViewerHCI::panUp);
  _fsa->addArc("PanDown", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_PanDown", "kbd_DOWN_down", false)));
  _fsa->addArcCallback("PanDown", this, &ViewerHCI::panDown);
  _fsa->addArc("PanLeft", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_PanLeft", "kbd_LEFT_down", false)));
  _fsa->addArcCallback("PanLeft", this, &ViewerHCI::panLeft);
  _fsa->addArc("PanRight", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_PanRight", "kbd_RIGHT_down", false)));
  _fsa->addArcCallback("PanRight", this, &ViewerHCI::panRight);
  
  _fsa->addArc("SpeedUpRock", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_SpeedUpRocking", "kbd_F_down", false)));
  _fsa->addArcCallback("SpeedUpRock", this, &ViewerHCI::speedUpRocking);
  _fsa->addArc("SlowDownRock", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_SlowDownRocking", "kbd_S_down", false)));
  _fsa->addArcCallback("SlowDownRock", this, &ViewerHCI::slowDownRocking);

  _fsa->addArc("IncRockAng", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_IncRockingAngle", "kbd_W_down", false)));
  _fsa->addArcCallback("IncRockAng", this, &ViewerHCI::incRockingAngle);
  _fsa->addArc("DecRockAng", "Start", "Start",
       splitStringIntoArray(ConfigVal("ViewerHCI_DecRockingAngle", "kbd_N_down", false)));
  _fsa->addArcCallback("DecRockAng", this, &ViewerHCI::decRockingAngle);
  

 _eventMgr->addFsaRef(_fsa);
}

ViewerHCI::~ViewerHCI()
{
}


void
ViewerHCI::trackballOn(MinVR::EventRef e)
{
    _lastIntersectionPt = Vector3::inf();
}

void
ViewerHCI::trackballMove(EventRef e)
{
  Vector2 newPos = e->get2DData();

  // All coordinates here in RoomSpace
  Vector3 orig = _gfxMgr->getCamera()->getCameraPos();
  Vector3 dir = (_gfxMgr->screenPointToRoomSpaceFilmplane(newPos) - orig).unit();
  Ray r = Ray::fromOriginAndDirection(orig, dir);

  double t = r.intersectionTime(_boundingSphere);
  if ((isFinite(t)) && (t > 0)) {
    Vector3 intersectionPt = orig + t*dir;

    if (_lastIntersectionPt != Vector3::inf()) {
      Vector3 v1 = (_lastIntersectionPt - _boundingSphere.center).unit();
      Vector3 v2 = (intersectionPt - _boundingSphere.center).unit();
      Vector3 rotAxis = v1.cross(v2).unit();
      double dot = v1.dot(v2);
      double angle = -aCos(dot);

      if ((rotAxis.isFinite()) && (!rotAxis.isZero())) {
        Matrix3 rot = Matrix3::fromAxisAngle(rotAxis, angle);
        rot.orthonormalize();
        CoordinateFrame frame = CoordinateFrame(_boundingSphere.center) * 
          CoordinateFrame(rot, Vector3::zero()) *
          CoordinateFrame(-_boundingSphere.center);
        _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * frame);
      }
    }
    _lastIntersectionPt = intersectionPt;
  }
  else {
    _lastIntersectionPt = Vector3::inf();
  }
}

void
ViewerHCI::trackballOff(EventRef e)
{
}


void
ViewerHCI::toggleRocking(EventRef e)
{
  _rocking = !_rocking;
  if (_rocking) {
    _lastRotTime = -1;
    _animationCallbackID = _gfxMgr->addPoseCallback(this, &ViewerHCI::poseForAnimation);
  }
  else {
    _gfxMgr->removePoseCallback(_animationCallbackID);
  }
}

void
ViewerHCI::poseForAnimation(Array<SurfaceRef> &posedModels, const CoordinateFrame &virtualToRoomSpace)
{
  double now = SynchedSystem::getLocalTime();
  if (_lastRotTime == -1) {
    _lastRotTime = now;
  }
  else {
    double deltaT = now - _lastRotTime;
    _lastRotTime = now;
    double angle = _rotAngularVel * deltaT;
    if (_rockingDir < 0.0) {
      angle = -angle;
    }
    _totalAngle += angle;
    if (fabs(_totalAngle) > _rockingAngle) {
      if (_totalAngle > 0.0) {
        _rockingDir = -1.0;
      }
      else {
        _rockingDir = 1.0;
      }
    }

    Vector3 rotAxis(0,1,0);
    Matrix3 r = Matrix3::fromAxisAngle(rotAxis, angle);
    CoordinateFrame f = CoordinateFrame(_boundingSphere.center) * 
      CoordinateFrame(r, Vector3::zero()) *
      CoordinateFrame(-_boundingSphere.center);
    _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * f); 
  }
}



void
ViewerHCI::panLeft(EventRef e)
{
  CoordinateFrame t(Vector3(_panAmt,0,0));
  _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * t);
}

void
ViewerHCI::panRight(EventRef e)
{
  CoordinateFrame t(Vector3(-_panAmt,0,0));
  _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * t);
}

void
ViewerHCI::panUp(EventRef e)
{
  CoordinateFrame t(Vector3(0,-_panAmt,0));
  _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * t);
}

void
ViewerHCI::panDown(EventRef e)
{
  CoordinateFrame t(Vector3(0,_panAmt,0));
  _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * t);
}

void
ViewerHCI::dollyIn(EventRef e)
{
  CoordinateFrame t(Vector3(0,0,-_panAmt));
  _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * t);
}

void
ViewerHCI::dollyOut(EventRef e)
{
  CoordinateFrame t(Vector3(0,0,_panAmt));
  _gfxMgr->setRoomToVirtualSpaceFrame(_gfxMgr->getRoomToVirtualSpaceFrame() * t);
}

void
ViewerHCI::scaleUp(EventRef e)
{
  _gfxMgr->setRoomToVirtualSpaceScale(0.95 * _gfxMgr->getRoomToVirtualSpaceScale());
}

void
ViewerHCI::scaleDown(EventRef e)
{
  _gfxMgr->setRoomToVirtualSpaceScale(1.05 * _gfxMgr->getRoomToVirtualSpaceScale());
}

void
ViewerHCI::speedUpRocking(EventRef e)
{
  _rotAngularVel += 0.05;
}

void
ViewerHCI::slowDownRocking(EventRef e)
{
  _rotAngularVel -= 0.05;
}

void
ViewerHCI::incRockingAngle(EventRef e)
{
  _rockingAngle += 0.05;
}

void
ViewerHCI::decRockingAngle(EventRef e)
{
  _rockingAngle -= 0.05;
}


} // namespace
