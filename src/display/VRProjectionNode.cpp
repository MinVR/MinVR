#include <display/VRProjectionNode.h>

#include <math.h>

namespace MinVR{

VRProjectionNode::VRProjectionNode(const std::string &name, float fovX, float fovY, float nearClip, float farClip): 
  VRDisplayNode(name), _fovX(fovX), _fovY(fovY), _nearClip(nearClip), _farClip(farClip)
{
  _valuesAdded.push_back("/ProjectionMatrix");
  _valuesAdded.push_back("/ViewMatrix");
  _valuesNeeded.push_back("/LookAtMatrix");
 
  double degreeToRadian = 3.1415926 / 180;
  float _horizontalClip = tan(fovX * degreeToRadian / 2.0f) * _nearClip;
  float _verticalClip = tan(fovY * degreeToRadian / 2.0f) * _nearClip;

  VRMatrix4 projMat = VRMatrix4::projection(-_horizontalClip, _horizontalClip, -_verticalClip, _verticalClip, _nearClip, _farClip); //TODO: This should just be cacheable once since it doesn't change.
 _projectionMatrix = projMat; 


}

VRProjectionNode::~VRProjectionNode()
{
}

void VRProjectionNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler)
{
  renderState->pushState();

  renderState->addData("/ProjectionMatrix", _projectionMatrix);

  VRMatrix4 viewMat;
  if (renderState->exists("/LookAtMatrix")){
    VRMatrix4 lookAtMatrix = renderState->getValue("/LookAtMatrix");
    lookAtMatrix = lookAtMatrix.inverse();
    viewMat = lookAtMatrix;
  }
  renderState->addData("/ViewMatrix", viewMat);

  VRDisplayNode::render(renderState, renderHandler);

  renderState->popState();

}

VRDisplayNode* VRProjectionNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace){
  float nearClip = config->getValue("NearClip", nameSpace);
  float farClip = config->getValue("FarClip", nameSpace);
  float fovY = config->getValue("FieldOfViewY", nameSpace);
  float fovX = config->getValue("FieldOfViewX", nameSpace);

  VRProjectionNode *node = new VRProjectionNode(nameSpace, fovX, fovY, nearClip, farClip);

  return node;
}

} // end namespace
