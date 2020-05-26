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
    //viewMatrix.setColumn(1, viewMatrix.column(1));
    viewMatrix.setColumn(3, viewMatrix.column(3) + G3D::Vector4(0,1,0.0,0));
    G3D::Matrix4 g3dPjMtx = rd->projectionMatrix();
    rd->setProjectionMatrix(projectionMtrx);
    rd->setCameraToWorldMatrix(viewMatrix.approxCoordinateFrame().inverse());

  }
}