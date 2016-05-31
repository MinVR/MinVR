/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/VRStereoNode.h>
#include <display/VRGroupNode.h>

namespace MinVR {

VRStereoNode::VRStereoNode(const std::string &name, VRGraphicsToolkit *gfxToolkit, VRStereoFormat format) :
	VRDisplayNode(name), _gfxToolkit(gfxToolkit), _format(format) {
}

VRStereoNode::~VRStereoNode() {
}


void VRStereoNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler) {
  renderState->pushState();

	if (_format == VRSTEREOFORMAT_MONO) {
		renderState->addData("StereoFormat", "Mono");
		renderState->addData("Eye", "Cyclops");
		_gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACK);
		renderOneEye(renderState, renderHandler);
	}
	else if (_format == VRSTEREOFORMAT_QUADBUFFERED) {
		renderState->addData("StereoFormat", "QuadBuffered");
		renderState->addData("Eye", "Left");
		_gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACKLEFT);
		renderOneEye(renderState, renderHandler);

		renderState->addData("Eye", "Right");
		_gfxToolkit->setDrawBuffer(VRGraphicsToolkit::VRDRAWBUFFER_BACKRIGHT);
		renderOneEye(renderState, renderHandler);
	}
	else if (_format == VRSTEREOFORMAT_SIDEBYSIDE) {
		renderState->addData("StereoFormat", "SideBySide");
		int x,y,w,h;
		if (renderState->exists("ViewportX", "/")) {
            x = renderState->getValue("ViewportX");
            y = renderState->getValue("ViewportY");
			w = renderState->getValue("ViewportWidth");
			h = renderState->getValue("ViewportHeight");
		}
		else {
            x = 0;
            y = 0;
			w = renderState->getValue("WindowWidth");
			h = renderState->getValue("WindowHeight");
		}
		renderState->addData("Eye", "Left");
		_gfxToolkit->setViewport(VRRect(x,y,w/2,h));
		renderOneEye(renderState, renderHandler);

		renderState->addData("Eye", "Right");
		_gfxToolkit->setViewport(VRRect(x+w/2+1,y,w/2,h));
		renderOneEye(renderState, renderHandler);
	}
	else if (_format == VRSTEREOFORMAT_COLUMNINTERLACED) {
		renderState->addData("StereoFormat", "ColumnInterlaced");
		renderState->addData("Eye", "Left");
		_gfxToolkit->disableDrawingOnEvenColumns();
		renderOneEye(renderState, renderHandler);

		renderState->addData("Eye", "Right");
		_gfxToolkit->disableDrawingOnOddColumns();
		renderOneEye(renderState, renderHandler);
		
		_gfxToolkit->enableDrawingOnAllColumns();
	}


  renderState->pushState();
}

void VRStereoNode::renderOneEye(VRDataIndex *renderState, VRRenderHandler *renderHandler)
{
	if (_children.size() > 0) {
		if (renderState->getTypeString("Eye") == "Left" || renderState->getTypeString("Eye") == "Cyclops"){
			_children[0]->render(renderState, renderHandler);
		} 
		else
		{
			_children[1]->render(renderState, renderHandler);
		}
	}
	else
	{
		renderHandler->onVRRenderScene(renderState, this);
	}
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
				config->pushState();
				config->addData(config->validateNameSpace(config->validateNameSpace(validatedNameSpace) + *it) + "Eye", "Left");
				VRDisplayNode *child = vrMain->getFactory()->createDisplayNode(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
				if (child != NULL) {
					child_left->addChild(child);
				}
				config->popState();
			}
		}
		VRDisplayNode *child_right = new VRGroupNode(validatedNameSpace + "_right");
		addChild(child_left);
		for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
			if (config->exists(*it, validatedNameSpace))
			{
				config->pushState();
				config->addData(config->validateNameSpace(config->validateNameSpace(validatedNameSpace) + *it) + "Eye", "Left");
				VRDisplayNode *child = vrMain->getFactory()->createDisplayNode(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
				if (child != NULL) {
					child_right->addChild(child);
				}
				config->popState();
			}
		}
	}
	else
	{
		for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
			if (config->exists(*it, validatedNameSpace)){

				config->pushState();
				config->addData(config->validateNameSpace(config->validateNameSpace(validatedNameSpace) + *it) + "Eye", "Cyclops");
				VRDisplayNode *child = vrMain->getFactory()->createDisplayNode(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
				if (child != NULL) {
					addChild(child);
				}
				config->popState();

			}
		}
	}

	//for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
	//	if (config->exists(*it, validatedNameSpace)){
	//		if (_format != VRSTEREOFORMAT_MONO)
	//		{
	//			config->pushState();
	//			config->addData(config->validateNameSpace(config->validateNameSpace(validatedNameSpace) + *it) + "Eye", "Left");
	//			VRDisplayNode *child_left = vrMain->getFactory()->createDisplayNode(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
	//			if (child_left != NULL) {
	//				addChild(child_left);	
	//			}
	//			config->popState();

	//			config->pushState();
	//			config->addData(config->validateNameSpace(config->validateNameSpace(validatedNameSpace) + *it) + "Eye", "Left");
	//			VRDisplayNode *child_right = vrMain->getFactory()->createDisplayNode(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
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
	//			VRDisplayNode *child = vrMain->getFactory()->createDisplayNode(vrMain, config, config->validateNameSpace(validatedNameSpace) + *it);
	//			if (child != NULL) {
	//				addChild(child);
	//			}
	//			config->popState();
	//			break;
	//		}
	//	}
	//}
}

VRDisplayNode* VRStereoNodeFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace)
{
 
	VRGraphicsToolkit *gfxToolkit = vrMain->getGraphicsToolkit(config->getValue("GraphicsToolkit", nameSpace));

	std::string formatStr = config->getValue("StereoFormat", nameSpace);
	VRStereoNode::VRStereoFormat format = VRStereoNode::VRSTEREOFORMAT_MONO;
	if (formatStr == "QuadBuffered") {
		format = VRStereoNode::VRSTEREOFORMAT_QUADBUFFERED;
	}
	else if (formatStr == "SideBySide") {
		format = VRStereoNode::VRSTEREOFORMAT_SIDEBYSIDE;
	}
	else if (formatStr == "ColumnInterlaced") {
		format = VRStereoNode::VRSTEREOFORMAT_COLUMNINTERLACED;
	}

	VRDisplayNode *node = new VRStereoNode(nameSpace, gfxToolkit, format);

	return node;
}

} /* namespace MinVR */
