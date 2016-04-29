
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
	VRMatrix4 headFrame = _headMatrix * VRMatrix4::translation(VRVector3(-_iod/2.0 + _iod*(eye), 0, 0));

	// Use the tile to calculate half values
	double halfWidth = (_topRight - _topLeft).length() / 2.0;
	double halfHeight = (_topRight - _botRight).length() / 2.0;

	// Calculate the center of the tile
	VRPoint3 center = _topLeft + 0.5*(_topRight - _topLeft) + 0.5*(_botLeft - _topLeft);
	
	// Calculate tile to room matrix
	VRVector3 x = (_topRight - _topLeft).normalize();
	VRVector3 y = (_topLeft - _botLeft).normalize();
	VRVector3 z = x.cross(y).normalize();
	VRMatrix4 tile2room(x.x, y.x, z.x, center.x,
						x.y, y.y, z.y, center.y,
						x.z, y.z, z.z, center.z,
						0, 0, 0, 1);

	// Calculate room to tile matrix
	VRMatrix4 room2tile = tile2room.inverse();

	// Calculate the camera frame and position
	VRMatrix4 camera2Room = headFrame;
	VRMatrix4 cameraFrame = room2tile * camera2Room;
	VRPoint3 cameraPos = VRPoint3(0,0,0) + cameraFrame.getColumn(3);
	
	// Calculate projection parameters
	double lcamera = (-halfWidth - cameraPos.x);
	double rcamera = (halfWidth - cameraPos.x);
	double b = (-halfHeight - cameraPos.y);
	double t = (halfHeight - cameraPos.y);
	double dist = cameraPos.z;
	double k = _nearClip / dist;

	// Calculate view matrix based on the camera position and room tile
	VRMatrix4 view = VRMatrix4::translation(VRPoint3(0,0,0) - cameraPos) * room2tile;

	static VRMatrix4 invertXYMat(1,  0, 0, 0,
						  0, -1, 0, 0,
						  0,  0, 1, 0,
						  0,  0, 0, 1);

	// Calculate off axis projection matrix
	VRMatrix4 projMat = invertXYMat * VRMatrix4::projection(lcamera*k, rcamera*k, b*k, t*k, _nearClip, _farClip);
	
	renderState->addData("ViewMatrix", view);
	renderState->addData("ProjectionMatrix", projMat);

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
VROffAxisProjectionNodeFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) {
	std::string nodeNameSpace = config->validateNameSpace(nameSpace + valName);

	std::string type = config->getValue("Type", nodeNameSpace);
	if (type != "VROffAxisProjectionNode") {
		// This factory cannot create the type specified
		return NULL;
	}

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
		VRDisplayNode *child = vrMain->getFactory()->createDisplayNode(vrMain, config, *it, "/MinVR/");
		if (child != NULL) {
			node->addChild(child);
		}
	  }
    }

	vrMain->addEventHandler(node);

	return node;
}


} // end namespace


