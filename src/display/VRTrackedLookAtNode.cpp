
#include <display/VRTrackedLookAtNode.h>

namespace MinVR {


	VRTrackedLookAtNode::VRTrackedLookAtNode(const std::string &name, const std::string &headTrackingEventName, VRMatrix4 initiallookAtMatrix) :
		VRDisplayNode(name), _lookAtMatrix(initiallookAtMatrix), _trackingEvent(headTrackingEventName)
{
  _valuesAdded.push_back("/LookAtMatrix");
}

VRTrackedLookAtNode::~VRTrackedLookAtNode()
{
}


void 
VRTrackedLookAtNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler)
{
	renderState->pushState();

	renderState->addData("/LookAtMatrix", _lookAtMatrix);

	VRDisplayNode::render(renderState, renderHandler);

	renderState->popState();
}

void
VRTrackedLookAtNode::onVREvent(const VREvent &e)
{
	if (e.getName() == _trackingEvent) {
		VRMatrix4 head_frame(e.getDataAsFloatArray("Transform"));
		_lookAtMatrix = head_frame.inverse();
	}
}

VRDisplayNode* VRTrackedLookAtNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {

	std::string trackingEvent = config->getValue("HeadTrackingEvent", nameSpace);
	VRMatrix4 lookAtMatrix;

	if (config->exists("LookAtMatrix", nameSpace)){
		lookAtMatrix = config->getValue("LookAtMatrix", nameSpace);
	}
	else if (config->exists("LookAtUp", nameSpace) && config->exists("LookAtEye", nameSpace) && config->exists("LookAtCenter", nameSpace))
	{
		VRVector3 up = config->getValue("LookAtUp", nameSpace);
		VRVector3 eye = config->getValue("LookAtEye", nameSpace);
		VRVector3 center = config->getValue("LookAtCenter", nameSpace);

		VRVector3 z = center - eye;
		z.normalize();
		VRVector3 x = up.cross(z);
		x.normalize();
		VRVector3 y = z.cross(x);

        VRMatrix4 M1 = VRMatrix4::fromRowMajorElements(x[0], y[0], z[0], 0,
                                                       x[1], y[1], z[1], 0,
                                                       x[2], y[2], z[2], 0,
                                                       0, 0, 0, 1);

        VRMatrix4 M2 = VRMatrix4::fromRowMajorElements(1, 0, 0, -eye[0],
                                                       0, 1, 0, -eye[1],
                                                       0, 0, 1, -eye[2],
                                                       0, 0, 0, 1);

		lookAtMatrix = M1 * M2;
	}
	else
	{
		std::cerr << "Warning : no LookAtMatrix defined for " << nameSpace << std::endl;
		std::cerr << "Either Define  LookAtMatrix or LookAtUp,LookAEye and LookAtCenter" << std::endl;
		std::cerr << "Using IdentityMatrix as default 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 " << std::endl;
	}

	VRTrackedLookAtNode *node = new VRTrackedLookAtNode(nameSpace, trackingEvent, lookAtMatrix);

	vrMain->addEventHandler(node);

	return node;
}


} // end namespace


