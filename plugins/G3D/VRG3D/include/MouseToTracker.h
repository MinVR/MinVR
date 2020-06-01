//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

/**
 * \author Daniel Keefe (dfk)
 *
 * \file  MouseToTracker.h
 * \brief Transforms mouse movement and keypresses into 6D tracker events.
 *        For use with testing 6D interaction techniques on a desktop.
 *
 */

#ifndef MOUSETOTRACKER_H
#define MOUSETOTRACKER_H

#include <G3D/G3D.h>
#include <GLG3D/GLG3D.h>
#include "Event.h"
#include "ProjectionVRCamera.h"

namespace MinVR {


typedef G3D::ReferenceCountedPointer<class MouseToTracker> MouseToTrackerRef;
/** Converts mouse movement and keypresses into CoordinateFrame events
    mimicking 6DOF Tracker input.  Useful primarily for debugging VR
    interactions at a desktop with a mouse.  If you hold down the
    X,Y,or Z key while moving the mouse you'll rotate the tracker
    around the corresponding local axis.  If you specify something >1
    for the numTrackers parameter, then you can press the TAB key to
    cycle through controlling several trackers.  The Event names for
    the trackers are Mouse1_Tracker, Mouse2_Tracker, etc..
*/
class MouseToTracker : public G3D::ReferenceCountedObject
{
public:

  PLUGIN_API MouseToTracker(ProjectionVRCameraRef camera, int numTrackers=1);

  PLUGIN_API virtual ~MouseToTracker();

  /// Call this method each frame to respond to input events that
  /// should include Mouse_Pointer events and kbd events.  It returns
  /// an array of tracker events generated based on the input.
  PLUGIN_API void doUserInput(const G3D::Array<EventRef> &events, G3D::Array<EventRef> &generatedEvents);

protected:

  std::string intToString(int i);
  void cycleTracker();

  EventRef mouseMoveInPlane(EventRef e);
  EventRef mouseMoveInOut(EventRef e);
  EventRef mouseMoveRotX(EventRef e);
  EventRef mouseMoveRotY(EventRef e);
  EventRef mouseMoveRotZ(EventRef e);


  enum FsaStates {
    START,
    XROT,
    YROT,
    ZROT,
    DOLLY
  };

  ProjectionVRCameraRef _camera;
  FsaStates     _fsaState;
  G3D::Matrix3       _rot;
  G3D::Vector3       _trans;
  G3D::Vector2       _lastPos;
  int           _numTrackers;
  int           _curTracker;
};

} // end namespace

#endif
