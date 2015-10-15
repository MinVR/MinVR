#ifndef VRCAMERAPAIR_H
#define VRCAMERAPAIR_H


#include "VRStereoFormatter.h"
#include "VREvent.h"

/** A Camera class similar to many computer graphics libraries but recognizing that VR cameras should typically
    be managed in pairs, with one camera for the left eye and one for the right.  In this abstraction, a single
    transformation matrix can be used to define the position and orientation of the "head" (really the centerpoint)
    between the two cameras), and an offset equal to the "eye separation" is used to separate the two cameras.
*/
class VRCameraPair {
public:

  enum ProjectionType {
    LEFT_EYE_PROJECTION,
    RIGHT_EYE_PROJECTION,
    MONO_PROJECTION
  };

  VRCameraPair();
  VRCameraPair(VRMat4 initialHeadTransform, float eyeSeparation);

  virtual ~VRCameraPair();

  virtual void handleUserInput(const std::vector<VREvent> &inputEvents);

  virtual VRMat4 getProjectionMatrix(ProjectionType projectionType);

private:

  VRMat4 _headTransform;
  float _eyeSeparation;

};

#endif

