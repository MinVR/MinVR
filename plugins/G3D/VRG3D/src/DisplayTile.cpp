//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

#include "../include/DisplayTile.h"

namespace MinVR {

using namespace G3D;

DisplayTile::DisplayTile(
    Vector3 topLeftCorner,
    Vector3 topRightCorner,
    Vector3 botLeftCorner,
    Vector3 botRightCorner,
    TileRenderType tileRenderType,
    double  nearClipPlane,
    double  farClipPlane,
    int viewportXpos,
    int viewportYpos,
    int viewportWidth,
    int viewportHeight)
{
  topLeft = topLeftCorner;
  topRight = topRightCorner;
  botLeft = botLeftCorner;
  botRight = botRightCorner;
  renderType = tileRenderType;
  nearClip = nearClipPlane;
  farClip = farClipPlane;
  viewportX = viewportXpos;
  viewportY = viewportYpos;
  viewportW = viewportWidth;
  viewportH = viewportHeight;

  calculateRoomToTile();
}

DisplayTile::~DisplayTile()
{
}

void
DisplayTile::calculateRoomToTile()
{
  Vector3 center = (topLeft + topRight + botLeft + botRight) / 4.0;
  Vector3 x = (topRight - topLeft).unit();
  Vector3 y = (topLeft - botLeft).unit();
  Vector3 z = x.cross(y).unit();
  Matrix3 rot(x[0],y[0],z[0],x[1],y[1],z[1],x[2],y[2],z[2]);
  CoordinateFrame tile2room(rot, center);
  room2tile = tile2room.inverse();
}

const DisplayTile&
DisplayTile::defaultDesktopTile()
{
  static DisplayTile d(Vector3(-0.65, 0.5, 0.0),
                       Vector3( 0.65, 0.5, 0.0),
                       Vector3(-0.65,-0.5, 0.0),
                       Vector3( 0.65,-0.5, 0.0),
                       TILE_MONO, 0.01, 100);
  return d;
}


} // end namespace

