#ifndef VRHEADTRACKEDPANELCAMERAPAIR_H
#define VRHEADTRACKEDPANELCAMERAPAIR_H

#include "VRCameraPair.h"

/**
*/
class VRHeadTrackedPanelCameraPair : public VRCameraPair {
public:

  VRHeadTrackedPanelCameraPair(VRVec3 topLeft, VRVec3 topRight, VRVec3 botRight, VRVec3 botLeft, VRMat4 initialHeadTransform, const std::string &headTrackingEventName);
  virtual ~VRHeadTrackedPanelCameraPair();

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

