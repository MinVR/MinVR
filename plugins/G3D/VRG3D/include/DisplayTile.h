//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

/**
 * \author Daniel Keefe (dfk)
 *
 * \file  DisplayTile.h
 *
 */


#ifndef DISPLAYTILE_H
#define DISPLAYTILE_H

#include <G3D/G3D.h>
#include <GLG3D/GLG3D.h>

namespace MinVR {

/** A description of the display screen for projection-based VR
    systems.  This is used together with a head position and
    orientation to setup the appropriate projection matrix for
    head-tracked rendering.

    Important: After setting or changing any of the variables that
    define the postion of the tile (top*, bot*), calculateRoomToTile()
    needs to be called to update the room2tile matrix.
*/
class DisplayTile
{
public:

  enum TileRenderType {
    TILE_MONO,                // normal mono rendering
    TILE_MONO_LEFT,           // mono rendering of left eye only
    TILE_MONO_RIGHT,          // mono rendering of right eye only
    TILE_STEREO,              // opengl quad buffered stereo
    TILE_CHECKERBOARD_STEREO,  // checkerboard pattern encoded stereo (samsung tv display)
    TILE_BLUELINE_STEREO,
    TILE_HORIZONTAL_STEREO,     // horizontal stripe pattern encoded stereo (jvc display)
    TILE_SIDE_BY_SIDE           // side by side (panasonic display)
  };


  /// Sets the state of the tile and calls calculateRoomToTile.  The viewport
  /// parameters only have an effect in mulit-tile configurations.
  DisplayTile(G3D::Vector3 topLeftCorner,
              G3D::Vector3 topRightCorner,
              G3D::Vector3 botLeftCorner,
              G3D::Vector3 botRightCorner,
              TileRenderType tileRenderType,
              double  nearClipPlane,
              double  farClipPlane,
              int viewportXpos = 0,
              int viewportYpos = 0,
              int viewportWidth = 0,
              int viewportHeight = 0);

  DisplayTile() {}

  virtual ~DisplayTile();

  TileRenderType  renderType;
  double          nearClip;
  double          farClip;
  G3D::Vector3         topLeft;
  G3D::Vector3         topRight;
  G3D::Vector3         botLeft;
  G3D::Vector3         botRight;

  // These are only needed in multi-tile situations where each displaytile
  // is rendered in a different gl viewport
  int             viewportX;
  int             viewportY;
  int             viewportW;
  int             viewportH;
  
  /** Calculates the appropriate room2tile matrix based on the
      location of the tile.  room2tile is cached in this class for
      speed and used by ProjectionVRCamera each frame.  If you change
      the position of the tile, you should call this to recalculate
      the room2tile matrix. */
  void            calculateRoomToTile();
  G3D::CoordinateFrame room2tile;

  /// Default setup for a desktop display
  static const DisplayTile& defaultDesktopTile();

};

} // end namespace


#endif
