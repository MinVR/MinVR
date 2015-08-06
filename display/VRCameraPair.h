#ifndef VRCAMERARIG_H
#define VRCAMERARIG_H


#include "VRStereoFormatter.h"
#include "VREvent.h"

/** Interface for different methods of handling head-tracked perspective projection for VR.
*/
class VRCameraRig {
public:

  enum ProjectionType {
    LEFT_EYE_PROJECTION,
    RIGHT_EYE_PROJECTION,
    MONO_PROJECTION
  };

  VRCameraRig();
  virtual ~VRCameraRig();

  virtual void handleUserInput(const std::vector<VREvent> &inputEvents);

  virtual float* getProjectionMatrix(ProjectionType projectionType);

private:


};

#endif

