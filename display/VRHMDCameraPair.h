#ifndef VRHMDCAMERAPAIR_H
#define VRHMDCAMERAPAIR_H

#include "VRCameraPair.h"

/**
*/
class VRHMDCameraPair : public VRCameraPair {
public:

  VRHMDCameraPair(float horizFieldOfView, float aspectRatio, VRMat4 initialHeadTransform, float eyeSeparation, const std::string &headTrackingEventName);
  virtual ~VRHMDCameraPair();

  virtual void handleUserInput(const std::vector<VREvent> &inputEvents);

  virtual VRMat4 getProjectionMatrix(ProjectionType type) {
  	if (projectionType == LEFT_EYE_PROJECTION) {
      // calculate projection matrix based on latest head tracking data
    }
    else if (projectionType == RIGHT_EYE_PROJECTION) {
      // calculate projection matrix based on latest head tracking data
    }
    else {
      // calculate projection matrix based on latest head tracking data
    }
  }

private:


};

#endif

