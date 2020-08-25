#include "../include/ProjectionVRCamera.h"
#include <GLG3D/Framebuffer.h>
namespace MinVR {
   
  ProjectionVRCamera::ProjectionVRCamera(G3D::Vector3 topLeftCorner,
    G3D::Vector3 topRightCorner,
    G3D::Vector3 botLeftCorner,
    G3D::Vector3 botRightCorner, ViewConfiguration cameraViewConfiguration,
    float nearClip,
    float farClip,
    int viewportXpos,
    int viewportYpos,
    int viewportWidth,
    int viewportHeight,
    double interOcularDist):
    _topLeftCorner(topRightCorner),
    _topRightCorner(topRightCorner),
    _botLeftCorner(botLeftCorner),
    _botRightCorner(botRightCorner), _nearClip(nearClip), _farClip(farClip),iod(interOcularDist),
    viewConfiguration(cameraViewConfiguration),
    _viewportX(viewportXpos),
    _viewportY(viewportYpos),
    _viewportW(viewportWidth),
    _viewportH(viewportHeight)
  {
    
  }
  
  ProjectionVRCamera::ProjectionVRCamera(ViewConfiguration cameraViewConfiguration,
    double interOcularDist):
    iod(interOcularDist),
    viewConfiguration(cameraViewConfiguration)
  {
  
  }

  ProjectionVRCamera::ProjectionVRCamera(float left, float right, float bottom, float top, float nearClip, float farClip, ViewConfiguration cameraViewConfiguration, double interOcularDist)
    :_left(left), _right(right), _bottom(bottom),
    _top(top), _nearClip(nearClip), _farClip(farClip), iod(interOcularDist),
    viewConfiguration(cameraViewConfiguration)
  {

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
    G3D::CoordinateFrame tcf = viewMatrix.approxCoordinateFrame();
    //viewMatrix.setColumn(3, viewMatrix.column(3) + G3D::Vector4(0, 1, 0.0, 0));
    G3D::Matrix4 g3dPjMtx = rd->projectionMatrix();

    rd->setProjectionMatrix(rd->invertYMatrix() * projectionMtrx);

    /*if (viewConfiguration == VR)
    {
      // the openvr view matrix is already inverted
      rd->setCameraToWorldMatrix(tcf);
    }
    else
    {
      rd->setCameraToWorldMatrix(tcf.inverse());
    }*/
    rd->setCameraToWorldMatrix(tcf.inverse());
    
    //projectionMtrx = projectionMtrx.transpose();
    //const float* vMatrix = state.getViewMatrix();
    //G3D::Matrix4 viewMatrix = state.getViewMatrix();
    //G3D::Matrix4 viewTransposed = viewMatrix.transpose();
    //G3D::CoordinateFrame cf = viewMatrix.approxCoordinateFrame();
    //G3D::CoordinateFrame tcf = viewTransposed.approxCoordinateFrame();
    ////G3D::Vector3 cf.translation;
    ////std::cout << "View matrix: " << cf.translation << std::endl;
    ////std::cout << "Transposed View matrix: " << tcf.translation << std::endl;
    //const float* vCamPos = state.getCameraPos();

    //G3D::Vector3 camPos(vCamPos[0], vCamPos[1], vCamPos[2]);
    ////std::cout << "CamPos: " <<  camPos << std::endl;
    ////camPos += G3D::Vector3(0, 0, -1.0);

    ////viewTransposed.setRow(1, viewTransposed.column(1) * -1);
    ////viewTransposed.setColumn(3,  G3D::Vector4(camPos.x, camPos.y, camPos.z,1.0));
    ////viewTransposed.setColumn(3, viewTransposed.column(3) );
    ////G3D::Matrix4 g3dPjMtx = rd->projectionMatrix();
    ////viewTransposed.setColumn(3, G3D::Vector4(camPos.x, camPos.y, camPos.z, 1.0));

    ////position
    //G3D::Matrix4 translateM = G3D::Matrix4::translation(0, 0, -1);
    ////viewTransposed = viewTransposed * translateM;
    ////viewTransposed = viewTransposed.translation(0, 0, -1);
    ////viewTransposed.setColumn(3, G3D::Vector4(0, 0, -1, 1.0));
    ////G3D::Vector3 upVector(viewTransposed[0][1], viewTransposed[1][1], viewTransposed[2][1]);
    ////upVector *= 1;
    ////viewTransposed.setColumn(1, G3D::Vector4(upVector.x, upVector.y, upVector.z, 0.0));
    ////viewTransposed.setColumn(3, G3D::Vector4(0, 0, 1, 0));
    ////viewTransposed.setColumn(0, -viewTransposed.column(0));
    ////viewTransposed.setColumn(2, -viewTransposed.column(2));
    //G3D::CoordinateFrame viewCoordinateF = viewMatrix.approxCoordinateFrame();
    //G3D::CoordinateFrame viewTransposedCoordinateF = viewTransposed.approxCoordinateFrame();

    ////G3D::Vector3 up = viewTransposedCoordinateF.upVector();
    //G3D::Matrix4 M(1, 0, 0, 0,
    //  0, -1, 0, 0,
    //  0, 0, 1, 0,
    //  0, 0, 0, 1);


    ////rd->pushState(fbo);
    //rd->setProjectionMatrix(projectionMtrx);


    ////rd->setProjectionMatrix(M * projectionMtrx);
    ////rd->setProjectionMatrix(projectionMtrx);
    //rd->setCameraToWorldMatrix(viewTransposedCoordinateF);
    ////rd->clear();
    ////rd->popState();
  }

  void ProjectionVRCamera::setViewMatrixCoordinate(G3D::Matrix4& viewMatrix)
  {
    headFrame = viewMatrix.approxCoordinateFrame();
  }

  PLUGIN_API void ProjectionVRCamera::setProjectionMatrix(G3D::Matrix4& projectionM)
  {
    projectionMatrix = projectionM;
  }

  PLUGIN_API void ProjectionVRCamera::getInverseViewProjection(G3D::Matrix4& inverseMatrix)
  {
    inverseMatrix = (projectionMatrix * headFrame.toMatrix4()).inverse().toMatrix4();
  }

}