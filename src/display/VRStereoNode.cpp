/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/VRStereoNode.h>
#include <display/VRGroupNode.h>
#include <math/VRMath.h>

namespace MinVR {

VRStereoNode::VRStereoNode(const std::string &name, float interOcularDist, VRGraphicsToolkit *gfxToolkit, VRStereoFormat format) :
  VRDisplayNode(name), _gfxToolkit(gfxToolkit), _format(format), _iod(interOcularDist) {
  _valuesAdded.push_back("/StereoFormat");
  _valuesAdded.push_back("/LookAtMatrix");
  _valuesAdded.push_back("/Eye");
}

VRStereoNode::~VRStereoNode() {
}


void VRStereoNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler) {
  renderState->pushState();

	if (_format == VRSTEREOFORMAT_MONO) {
		renderState->addData("/StereoFormat", "Mono");

		_gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACK);
		renderOneEye(renderState, renderHandler, Cyclops);
	}
	else if (_format == VRSTEREOFORMAT_QUADBUFFERED) {
		renderState->addData("/StereoFormat", "QuadBuffered");

		_gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACKLEFT);
		renderOneEye(renderState, renderHandler, Left);

		_gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACKRIGHT);
		renderOneEye(renderState, renderHandler, Right);
	}
	else if (_format == VRSTEREOFORMAT_SIDEBYSIDE) {
		renderState->addData("/StereoFormat", "SideBySide");

		int x,y,w,h;
		if (renderState->exists("/ViewportX")) {
            x = renderState->getValue("/ViewportX");
            y = renderState->getValue("/ViewportY");
			w = renderState->getValue("/ViewportWidth");
			h = renderState->getValue("/ViewportHeight");
		}
		else {
            x = 0;
            y = 0;
			w = renderState->getValue("/WindowWidth");
			h = renderState->getValue("/WindowHeight");
		}

		_gfxToolkit->setSubWindow(VRRect(x,y,w/2,h));
		renderOneEye(renderState, renderHandler, Left);

		_gfxToolkit->setSubWindow(VRRect(x + w / 2 + 1, y, w / 2, h));
		renderOneEye(renderState, renderHandler, Right);
	}
	else if (_format == VRSTEREOFORMAT_COLUMNINTERLACED) {
		renderState->addData("/StereoFormat", "ColumnInterlaced");

		_gfxToolkit->disableDrawingOnEvenColumns();
		renderOneEye(renderState, renderHandler, Left);

		_gfxToolkit->disableDrawingOnOddColumns();
		renderOneEye(renderState, renderHandler, Right);

		_gfxToolkit->enableDrawingOnAllColumns();
	}

	renderState->popState();
}

void VRStereoNode::updateLookAtMatrix(VRDataIndex *renderState, VREyePosition eye)
{

	VRMatrix4 lookAtMatrix;

	if (renderState->exists("/LookAtMatrix"))
  {
		lookAtMatrix = renderState->getValue("/LookAtMatrix");
	}

	if (eye == Left)
	{
		VRMatrix4 head_frame = lookAtMatrix.inverse();
		head_frame = head_frame * VRMatrix4::translation(VRVector3(-_iod / 2.0, 0, 0));
		lookAtMatrix = head_frame.inverse();
	}
	else if (eye == Right)
	{
		VRMatrix4 head_frame = lookAtMatrix.inverse();
		head_frame = head_frame * VRMatrix4::translation(VRVector3(_iod / 2.0, 0, 0));
		lookAtMatrix = head_frame.inverse();
	}

	renderState->addData("/LookAtMatrix", lookAtMatrix);
}

void VRStereoNode::renderOneEye(VRDataIndex *renderState, VRRenderHandler *renderHandler, VREyePosition eye)
{
	renderState->pushState();
		updateLookAtMatrix(renderState, eye);
		if (_children.size() > 0) {
			if (eye == Cyclops)
			{
				renderState->addData("/Eye", "Cyclops");
				_children[0]->render(renderState, renderHandler);
			}
			else if (eye == Left){
				renderState->addData("/Eye", "Left");
				_children[0]->render(renderState, renderHandler);
			}
			else if (eye == Right)
			{
				renderState->addData("/Eye", "Right");
				_children[1]->render(renderState, renderHandler);
			}
		}
		else
		{
			renderHandler->onVRRenderScene(renderState, this);
		}
	renderState->popState();
}

void VRStereoNode::createChildren(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
	std::list<std::string> names = config->getValue(nameSpace);
	std::string validatedNameSpace = config->validateNameSpace(nameSpace);

	if (_format != VRSTEREOFORMAT_MONO)
	{
		VRDisplayNode *child_left = new VRGroupNode(validatedNameSpace + "_left");
		addChild(child_left);
		for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
			if (config->exists(*it, validatedNameSpace))
			{
				if (config->hasAttribute(validatedNameSpace + *it, "displaynodeType")){
					config->pushState();
					config->addData(validatedNameSpace + *it + "/Eye", "Left");
					VRDisplayNode *child = vrMain->getFactory()->create<VRDisplayNode>(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
					if (child != NULL) {
						child_left->addChild(child);
					}
					config->popState();
				}
			}
		}
		VRDisplayNode *child_right = new VRGroupNode(validatedNameSpace + "_right");
		addChild(child_right);
		for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
			if (config->exists(*it, validatedNameSpace))
			{
				if (config->hasAttribute(validatedNameSpace + *it, "displaynodeType")){
					config->pushState();
					config->addData(validatedNameSpace + *it + "/Eye", "Left");
					VRDisplayNode *child = vrMain->getFactory()->create<VRDisplayNode>(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
					if (child != NULL) {
						child_right->addChild(child);
					}
					config->popState();
				}
			}
		}
	}
	else
	{
		for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
			if (config->exists(*it, validatedNameSpace)){
				if (config->hasAttribute(validatedNameSpace + *it, "displaynodeType")){
					config->pushState();
					config->addData(validatedNameSpace + *it + "/Eye", "Cyclops");
					VRDisplayNode *child = vrMain->getFactory()->create<VRDisplayNode>(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
					if (child != NULL) {
						addChild(child);
					}
					config->popState();
				}
			}
		}
	}

	//for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
	//	if (config->exists(*it, validatedNameSpace)){
	//		if (_format != VRSTEREOFORMAT_MONO)
	//		{
	//			config->pushState();
	//			config->addData(config->validateNameSpace(config->validateNameSpace(validatedNameSpace) + *it) + "Eye", "Left");
	//			VRDisplayNode *child_left = vrMain->getFactory()->create<VRDisplayNode>(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
	//			if (child_left != NULL) {
	//				addChild(child_left);
	//			}
	//			config->popState();

	//			config->pushState();
	//			config->addData(config->validateNameSpace(config->validateNameSpace(validatedNameSpace) + *it) + "Eye", "Left");
	//			VRDisplayNode *child_right = vrMain->getFactory()->create<VRDisplayNode>(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
	//			if (child_right != NULL) {
	//				addChild(child_right);
	//			}
	//			config->popState();
	//			break;
	//		}
	//		else
	//		{
	//			config->pushState();
	//			config->addData(config->validateNameSpace(config->validateNameSpace(validatedNameSpace) + *it) + "Eye", "Cyclops");
	//			VRDisplayNode *child = vrMain->getFactory()->create<VRDisplayNode>(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
	//			if (child != NULL) {
	//				addChild(child);
	//			}
	//			config->popState();
	//			break;
	//		}
	//	}
	//}
}

VRDisplayNode* VRStereoNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace)
{

	VRGraphicsToolkit *gfxToolkit = vrMain->getGraphicsToolkit(config->getValue("GraphicsToolkit", nameSpace));
	float iod = 0.0f;
	std::string formatStr = config->getValue("StereoFormat", nameSpace);
	VRStereoNode::VRStereoFormat format = VRStereoNode::VRSTEREOFORMAT_MONO;
	if (formatStr == "QuadBuffered") {
		format = VRStereoNode::VRSTEREOFORMAT_QUADBUFFERED;
		iod = (float)config->getValue("EyeSeparation", nameSpace);
	}
	else if (formatStr == "SideBySide") {
		format = VRStereoNode::VRSTEREOFORMAT_SIDEBYSIDE;
		iod = (float)config->getValue("EyeSeparation", nameSpace);
	}
	else if (formatStr == "ColumnInterlaced") {
		format = VRStereoNode::VRSTEREOFORMAT_COLUMNINTERLACED;
		iod = (float)config->getValue("EyeSeparation", nameSpace);
	}

	VRDisplayNode *node = new VRStereoNode(nameSpace, iod, gfxToolkit, format);

	return node;
}

} /* namespace MinVR */
