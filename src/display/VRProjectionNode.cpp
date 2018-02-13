#include <display/VRProjectionNode.h>

#include <math.h>

namespace MinVR{

VRProjectionNode::VRProjectionNode(const std::string &name, float fovX, float fovY, float nearClip, float farClip):
  VRDisplayNode(name), _fovX(fovX), _fovY(fovY), _nearClip(nearClip), _farClip(farClip)
{
  // in:
  _addValueNeeded("CameraMatrix");
  // These should be recorded as necessary, and the auditValues()
  // method needs to be changed to accommodate not just the values
  // added by nodes in teh display tree, but also from the config
  // files.
  // _addValueNeeded("NearClip");
  // _addValueNeeded("FarClip");
  // _addValueNeeded("FieldOfViewY");
  // _addValueNeeded("FieldOfViewX");

  // out:
  _valuesAdded.push_back("ViewMatrix");
  _valuesAdded.push_back("ProjectionMatrix");

  float degreeToRadian = 3.1415926f / 180;
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

  renderState->addData("ProjectionMatrix", _projectionMatrix);

  VRMatrix4 cameraMat = renderState->getValue("CameraMatrix");

  VRMatrix4 viewMat = cameraMat.inverse();

  renderState->addData("ViewMatrix", viewMat);

  VRDisplayNode::render(renderState, renderHandler);

  renderState->popState();

}

VRDisplayNode* VRProjectionNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace){
  float nearClip = config->getValueWithDefault("NearClip", 0.001f, nameSpace);
  float farClip = config->getValueWithDefault("FarClip", 500.0f, nameSpace);
  float fovY = config->getValueWithDefault("FieldOfViewY", 60.0f, nameSpace);
  float fovX = config->getValueWithDefault("FieldOfViewX", 60.0f, nameSpace);

  VRProjectionNode *node = new VRProjectionNode(nameSpace, fovX, fovY, nearClip, farClip);

  return node;
}

} // end namespace
