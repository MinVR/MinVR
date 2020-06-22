#ifndef PROJECTIONVRCAMERA_H
#define PROJECTIONVRCAMERA_H

#include "CommonInc.H"

#include "DisplayTile.h"

namespace MinVR {

  typedef G3D::ReferenceCountedPointer<class ProjectionVRCamera> ProjectionVRCameraRef;
  /**
  */
  class ProjectionVRCamera : public G3D::ReferenceCountedObject
  {
  public:

    enum EyeProjectionType {
      LeftEye,             // left eye for quad-buffered stereo rendering
      RightEye,            // right eye for quad-buffered stereo rendering
      Cyclops,             // a single "eye" for mono rendering only
      LeftEyeCheckerboard, // left eye for checkerboard stereo rendering (samsung tv display)
      RightEyeCheckerboard // right eye for checkerboard stereo rendering (samsung tv display)
    };

    /** Inter-ocular distance defaults to 2.5 inches = 0.2083 feet, this
        assumes your virtual coordinates are reported in feet.
    */
    ProjectionVRCamera(float left, float right,
      float bottom, float top,
      float near, float far,
      double interOcularDist = 0.2083);

    ProjectionVRCamera(double interOcularDist = 0.2083);

    virtual ~ProjectionVRCamera();



    /** Sets the projection matrix and the camera to world matrix for
        the RenderDevice based on the current DisplayTile, HeadFrame,
        and which eye is about to be rendered. */
    PLUGIN_API void applyProjection(G3D::RenderDevice *rd, const VRGraphicsState& state);

    void getCurrentMatrices(G3D::Matrix4  &view, G3D::Matrix4  &projection,
      G3D::Matrix4  &viewLeft, G3D::Matrix4  &projectionLeft,
      G3D::Matrix4  &viewRight, G3D::Matrix4  &projectionRight);

    PLUGIN_API void setViewMatrixCoordinate(G3D::Matrix4& viewMatrix);
    PLUGIN_API void setProjectionMatrix(G3D::Matrix4& projectionMatrix);
    PLUGIN_API void getInverseViewProjection(G3D::Matrix4& inverseMatrix);

    G3D::CoordinateFrame getHeadFrame() { return headFrame; }
    G3D::CoordinateFrame getLeftEyeFrame();
    G3D::CoordinateFrame getRightEyeFrame();
    G3D::Vector3         getHeadPos() { return headFrame.translation; }
    G3D::Vector3         getCameraPos() { return headFrame.translation; }
    G3D::Vector3         getLookVec();
    DisplayTile     getTile() { return tile; }

    void updateHeadFrame(G3D::CoordinateFrame frame) { headFrame = frame; }
    void setDisplayTile(DisplayTile dispTile) { tile = dispTile; }

    EyeProjectionType  getLastProjectionEye() { return lastProjectionEye; }

    void   set_iod(double val) { iod = val; }
    double get_iod() const { return iod; }

    /// Inter-ocular distance, default = 2.5 inches = 0.2083 feet
    double           iod;
    DisplayTile      tile;
    float _left;
    float _right;
    float _bottom;
    float _top;
    float _near;
    float _far;
    G3D::CoordinateFrame  headFrame;
    EyeProjectionType lastProjectionEye;
    G3D::Matrix projectionMatrix;
    const G3D::Framebuffer::Ref& fbo;
  };

} // end namespace


#endif