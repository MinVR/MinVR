
#include <display/VROffAxisProjectionNode.h>

namespace MinVR {


VROffAxisProjectionNode::VROffAxisProjectionNode(const std::string &name, VRVector3 topLeft, VRVector3 botLeft, VRVector3 topRight, VRVector3 botRight, 
		float interOcularDist, const std::string &headTrackingEventName, VRMatrix4 initialHeadMatrix) : 
	VRDisplayNode(name), _topLeft(topLeft), _botLeft(botLeft), _topRight(topRight), _botRight(botRight), 
		_iod(interOcularDist), _headMatrix(initialHeadMatrix), _trackingEvent(headTrackingEventName)
{
}

VROffAxisProjectionNode::~VROffAxisProjectionNode()
{
}


void 
VROffAxisProjectionNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler)
{
  //renderState->pushState();

  //VRVector3 cameraPos = _headMatrix.getColumn(3);
    if (std::string(renderState->getValue("Eye", "/")) == "Left") {
		// TODO: offset to the left by 1/2 the ioc

	}
    else if (std::string(renderState->getValue("Eye", "/")) == "Right") {
		// TODO: offset to the right by 1/2 the ioc
		
	}

	VRMatrix4 projMat;
	// TODO: calculate the projection matrix

	renderState->addData("ProjectionMatrix", projMat);

	if (_children.size() == 0) {
		renderHandler->onVRRenderScene(renderState, this);
	}
	else {
		VRDisplayNode::render(renderState, renderHandler);
	}	

  //renderState->popState();
}

void
VROffAxisProjectionNode::onVREvent(const std::string &eventName, VRDataIndex *eventData)
{
	if (eventName == _trackingEvent) {
		_headMatrix = eventData->getValue("Transform", "/");
	}
}



VRDisplayNode*
VROffAxisProjectionNodeFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) {
	std::string nodeNameSpace = nameSpace + "/" + valName;

	std::string type = config->getValue("Type", nodeNameSpace);
	if (type != "VROffAxisProjectionNode") {
		// This factory cannot create the type specified
		return NULL;
	}

	VRVector3 topLeft = config->getValue("TopLeft", nodeNameSpace);
	VRVector3 botLeft = config->getValue("BotLeft", nodeNameSpace);
	VRVector3 topRight = config->getValue("TopRight", nodeNameSpace);
	VRVector3 botRight = config->getValue("BotRight", nodeNameSpace);
	float iod = (double)config->getValue("EyeSeparation", nodeNameSpace);
	// TODO: NearClip
	// TODO: FarClip
	std::string trackingEvent = config->getValue("HeadTrackingEvent", nodeNameSpace);
	VRMatrix4 headMatrix = config->getValue("InitialHeadMatrix", nodeNameSpace);

	VROffAxisProjectionNode *node = new VROffAxisProjectionNode(valName, topLeft, botLeft, topRight, botRight, iod, trackingEvent, headMatrix);

	std::vector<std::string> childrenNames = config->getValue("Children", nameSpace);
	for (std::vector<std::string>::iterator it = childrenNames.begin(); it < childrenNames.end(); ++it) {
		VRDisplayNode *child = vrMain->getFactory()->createDisplayNode(vrMain, config, *it, "/");
		if (child != NULL) {
			node->addChild(child);
		}
	}

	vrMain->addEventHandler(node);

	return node;
}


} // end namespace


