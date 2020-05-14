#include "../include/ProjectionVRCamera.h"

namespace MinVR {
   
  ProjectionVRCamera::ProjectionVRCamera(double interOcularDist)
  {
    iod = interOcularDist;
  }
  
  ProjectionVRCamera::~ProjectionVRCamera()
  {
  
  }

  void ProjectionVRCamera::applyProjection(G3D::RenderDevice *rd
    , const VRGraphicsState& state)
  {
    G3D::Matrix4 projectionMtrx = state.getProjectionMatrix();
    projectionMtrx = projectionMtrx.transpose();
    const float* vMatrix = state.getViewMatrix();
    G3D::Matrix4 viewMatrix = state.getViewMatrix();
    viewMatrix = viewMatrix.transpose();
    viewMatrix.setRow(1, -viewMatrix.row(1));
    G3D::Matrix4 g3dPjMtx = rd->projectionMatrix();
    rd->setProjectionMatrix(projectionMtrx);
    rd->setCameraToWorldMatrix(viewMatrix.approxCoordinateFrame().inverse());

  }
}