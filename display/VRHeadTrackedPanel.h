#ifndef VRHEADTRACKEDPANEL_H
#define VRHEADTRACKEDPANEL_H

/**
*/
class VRHeadTrackedPanel : public VRProjectionMethod {
public:

  VRHeadTrackedPanel(VRVec3 topLeft, VRVec3 topRight, VRVec3 botRight, VRVec3 botLeft, const std::string &headTrackingEventName);
  virtual ~VRHeadTrackedPanel();

  virtual void handleUserInput(const std::vector<VREvent> &inputEvents);

  virtual void applyProjection(ProjectionType type) {
  	if (projectionType == LEFT_EYE_PROJECTION) {
      // calculate projection matrix based on latest head tracking data
      _stereoGraphicsToolkit->setProjectionMatrix(m);
    }
    else if (projectionType == RIGHT_EYE_PROJECTION) {
      // calculate projection matrix based on latest head tracking data
      _stereoGraphicsToolkit->setProjectionMatrix(m);
    }
    else {
      // calculate projection matrix based on latest head tracking data
      _stereoGraphicsToolkit->setProjectionMatrix(m);
    }
  }

private:


};

#endif

