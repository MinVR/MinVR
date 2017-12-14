
#include <display/VRHeadTrackingNode.h>

namespace MinVR {


VRHeadTrackingNode::VRHeadTrackingNode(const std::string &name, const std::string &headTrackingEventName, VRMatrix4 initialHeadMatrix) :
    VRDisplayNode(name), _headMatrix(initialHeadMatrix), _trackingEvent(headTrackingEventName)
{
  _valuesAdded.push_back("HeadMatrix");
  _valuesAdded.push_back("CameraMatrix");
}

VRHeadTrackingNode::~VRHeadTrackingNode()
{
}


void
VRHeadTrackingNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler)
{
	renderState->pushState();

	renderState->addData("HeadMatrix", _headMatrix);

  // We copy the head matrix into "CameraMatrix" in case this is only a mono
  // configuration and the two are the same thing.  We don't use a link because
  // a stereo configuration will overwrite the camera matrix.
	renderState->addData("CameraMatrix", _headMatrix);
	VRDisplayNode::render(renderState, renderHandler);

	renderState->popState();
}

void
VRHeadTrackingNode::onVREvent(const VRDataIndex &e)
{
	if (e.getName() == _trackingEvent) {
        _headMatrix = e.getValue("Transform");
    }
}

VRDisplayNode* VRHeadTrackingNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {

	std::string trackingEvent = config->getValue("HeadTrackingEvent", nameSpace);
	VRMatrix4 headMatrix;

	if (config->exists("HeadMatrix", nameSpace)){
		headMatrix = config->getValue("HeadMatrix", nameSpace);
	}
	else if (config->exists("LookAtUp", nameSpace) && config->exists("LookAtEye", nameSpace) && config->exists("LookAtCenter", nameSpace))
	{
		VRVector3 up = config->getValue("LookAtUp", nameSpace);
		VRVector3 eye = config->getValue("LookAtEye", nameSpace);
		VRVector3 center = config->getValue("LookAtCenter", nameSpace);

        VRVector3 z = eye - center;
		z = z.normalize();
		VRVector3 x = up.cross(z);
		x = x.normalize();
		VRVector3 y = z.cross(x);

        VRMatrix4 M1 = VRMatrix4::fromRowMajorElements(x[0], y[0], z[0], 0,
                                                       x[1], y[1], z[1], 0,
                                                       x[2], y[2], z[2], 0,
                                                       0, 0, 0, 1);

        VRMatrix4 M2 = VRMatrix4::fromRowMajorElements(1, 0, 0, -eye[0],
                                                       0, 1, 0, -eye[1],
                                                       0, 0, 1, -eye[2],
                                                       0, 0, 0, 1);

		VRMatrix4 lookAtMatrix = M1 * M2;
        headMatrix = lookAtMatrix.inverse();
	}
	else
	{
		std::cerr << "Warning : no HeadMatrix defined for " << nameSpace << std::endl;
		std::cerr << "Either Define HeadMatrix or LookAtUp, LookAtEye and LookAtCenter" << std::endl;
		std::cerr << "Using IdentityMatrix as default 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 " << std::endl;
	}

	VRHeadTrackingNode *node = new VRHeadTrackingNode(nameSpace, trackingEvent, headMatrix);

	vrMain->addEventHandler(node);

	return node;
}


} // end namespace


