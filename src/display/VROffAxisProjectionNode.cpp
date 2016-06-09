
#include <display/VROffAxisProjectionNode.h>

namespace MinVR {


	VROffAxisProjectionNode::VROffAxisProjectionNode(const std::string &name, VRPoint3 topLeft, VRPoint3 botLeft, VRPoint3 topRight, VRPoint3 botRight,
		float interOcularDist, const std::string &headTrackingEventName, VRMatrix4 initialHeadMatrix, double nearClip, double farClip) :
	VRDisplayNode(name), _topLeft(topLeft), _botLeft(botLeft), _topRight(topRight), _botRight(botRight), 
		_iod(interOcularDist), _headMatrix(initialHeadMatrix), _trackingEvent(headTrackingEventName), _nearClip(nearClip), _farClip(farClip)
{
}

VROffAxisProjectionNode::~VROffAxisProjectionNode()
{
}


void 
VROffAxisProjectionNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler)
{
	renderState->pushState();

	// Determine which eye is being used
	float eye = 0;
    if (std::string(renderState->getValue("Eye", "/")) == "Left") {
    	eye = 0;
	}
    else if (std::string(renderState->getValue("Eye", "/")) == "Right") {
    	eye = 1;
	}

	
	// Calculate headFrame offset based on inter ocular distance
	VRMatrix4 headFrame = _headMatrix * VRMatrix4::translation(VRVector3(-_iod / 2.0 + _iod*(eye), 0, 0));


	// This projection code follows the math described in this paper:
	// http://csc.lsu.edu/~kooima/pdfs/gen-perspective.pdf

	VRPoint3 pa = _botLeft;
	VRPoint3 pb = _botRight;
	VRPoint3 pc = _topLeft;
	VRPoint3 pe = VRPoint3(headFrame[3][0], headFrame[3][1], headFrame[3][2]);


	// Compute an orthonormal basis for the screen
	VRVector3 vr = (pb - pa).normalize();
	VRVector3 vu = (pc - pa).normalize();
	VRVector3 vn = vr.cross(vu).normalize();

	// Compute the screen corner vectors
	VRVector3 va = pa - pe;
	VRVector3 vb = pb - pe;
	VRVector3 vc = pc - pe;

	// Find the distance from the eye to screen plane
	double d = -va.dot(vn);

	// Find the extent of the perpendicular projection
	double l = vr.dot(va) * _nearClip / d;
	double r = vr.dot(vb) * _nearClip / d;
	double b = vu.dot(va) * _nearClip / d;
	double t = vu.dot(vc) * _nearClip / d;

	VRMatrix4 projMat = VRMatrix4::projection(l, r, b, t, _nearClip, _farClip);

	renderState->addData("ProjectionMatrix", projMat);

	// Rotate the projection to be non-perpendicular
	VRMatrix4 Mrot(vr[0], vr[1], vr[2], 0.0,
		vu[0], vu[1], vu[2], 0.0,
		vn[0], vn[1], vn[2], 0.0,
		0.0, 0.0, 0.0, 1.0);

	// Move the apex of the frustum to the origin
	VRMatrix4 Mtrans = VRMatrix4::translation(VRPoint3(0, 0, 0) - pe);

	VRMatrix4 viewMat = Mrot * Mtrans;

	renderState->addData("ViewMatrix", viewMat);

	if (_children.size() == 0) {
		renderHandler->onVRRenderScene(renderState, this);
	}
	else {
		VRDisplayNode::render(renderState, renderHandler);
	}	

	renderState->popState();
}

void
VROffAxisProjectionNode::onVREvent(const std::string &eventName, VRDataIndex *eventData)
{
	if (eventName == "/" + _trackingEvent) {
		_headMatrix = eventData->getValue("Transform", eventName);
	}
}



VRDisplayNode*
VROffAxisProjectionNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) {
	std::string nodeNameSpace = nameSpace;

	VRPoint3 topLeft = config->getValue("TopLeft", nodeNameSpace);
	VRPoint3 botLeft = config->getValue("BottomLeft", nodeNameSpace);
	VRPoint3 topRight = config->getValue("TopRight", nodeNameSpace);
	VRPoint3 botRight = config->getValue("BottomRight", nodeNameSpace);
	float iod = (double)config->getValue("EyeSeparation", nodeNameSpace);
	double nearClip = config->getValue("NearClip", nodeNameSpace);
	double farClip = config->getValue("FarClip", nodeNameSpace);
	std::string trackingEvent = config->getValue("HeadTrackingEvent", nodeNameSpace);
	VRMatrix4 headMatrix = config->getValue("InitialHeadMatrix", nodeNameSpace);

	VROffAxisProjectionNode *node = new VROffAxisProjectionNode(valName, topLeft, botLeft, topRight, botRight, iod, trackingEvent, headMatrix, nearClip, farClip);

    if (config->exists("Children", nodeNameSpace)) {

	  std::vector<std::string> childrenNames = config->getValue("Children", nodeNameSpace);
	  for (std::vector<std::string>::iterator it = childrenNames.begin(); it < childrenNames.end(); ++it) {
		VRDisplayNode *child = vrMain->getFactory()->create<VRDisplayNode>(vrMain, config, *it, "/MinVR/");
		if (child != NULL) {
			node->addChild(child);
		}
	  }
    }

	vrMain->addEventHandler(node);

	return node;
}


} // end namespace


