/**
 This file is part of the MinVR Open Source Project, which is developed and
 maintained collaboratively by the University of Minnesota and Brown University.
 
 Copyright (c) 2016 Regents of the University of Minnesota and Brown University.
 This software is distributed under the BSD-3 Clause license, which can be found
 at: MinVR/LICENSE.txt.
 
 Original Author(s) of this File:
  Josh Tveite, 2017, Brown University 
  
 Author(s) of Significant Updates/Modifications to the File:
  ...
 */




#ifndef VRPROJECTIONNODE_H
#define VRPROJECTIONNODE_H


#include <string>

#include <display/VRDisplayNode.h>
#include <main/VRFactory.h>
#include <math/VRMath.h>


namespace MinVR {

/**
 * Represents a projection node with a near and far clipping plane.
 * Generates View and Projection matrices from the CameraMatrix
 */
class VRProjectionNode : public VRDisplayNode {
public:
  VRProjectionNode(const std::string &name, float fovX, float fovY, float nearClip, float farClip);
  virtual ~VRProjectionNode();

  virtual std::string getType() const { return "VRProjectionNode"; }

  virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

  static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

protected:
  float _fovX;
  float _fovY;
  float _nearClip;
  float _farClip;
  //The previous fields aren't necessary to store after computing the matrix, but I could see scenarios where having them would be useful (querying clip plane, adjusting FOV) so I keep them around.
  VRMatrix4 _projectionMatrix;
};

}


#endif
