/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/concrete/BaseDisplayDevice.h>
#include "config/VRDataIndex.h"
#include "display/VRDisplayDevice.h"
#include "display/VRCallbackDisplayAction.h"
#include "display/VRCallbackDisplayFrameAction.h"
#include "display/VRCallbackRenderer.h"
#include "display/VRDisplayAction.h"
#include "display/VRDisplayFrameAction.h"
#include "display/VRFrameController.h"
#include "display/VRRenderer.h"
#include "display/VRRenderState.h"

namespace MinVR {

BaseDisplayDevice::BaseDisplayDevice()  : parent(NULL), _allowThreading(false) {
}

BaseDisplayDevice::~BaseDisplayDevice() {
}

int BaseDisplayDevice::getDisplayXOffset() { return parent != NULL ? parent->getDisplayXOffset() : 0; }
int BaseDisplayDevice::getDisplayYOffset() { return parent != NULL ? parent->getDisplayYOffset() : 0; }
int BaseDisplayDevice::getXOffset() { return parent != NULL ? parent->getXOffset() : 0; }
int BaseDisplayDevice::getYOffset() { return parent != NULL ? parent->getYOffset() : 0; }
int BaseDisplayDevice::getWidth() { return parent != NULL ? parent->getWidth() : 0; }
int BaseDisplayDevice::getHeight() { return parent != NULL ? parent->getHeight() : 0; }
bool BaseDisplayDevice::isOpen() { return parent != NULL ? parent->isOpen() : true; }
bool BaseDisplayDevice::allowThreading() { return _allowThreading; }
bool BaseDisplayDevice::allowGraphics() { return true; }
void BaseDisplayDevice::setAllowThreading(bool allowed) { this->_allowThreading = allowed; }

bool BaseDisplayDevice::isQuadbuffered()
{
	for (int f = 0; f < subDisplays.size(); f++)
	{
		if (subDisplays[f]->isQuadbuffered())
		{
			return true;
		}
	}

	return false;
}

VRDisplayDevice* BaseDisplayDevice::getParent() const {
	return parent;
}

void BaseDisplayDevice::setParent(VRDisplayDevice* parent) {
	this->parent = parent;
}

const std::vector<VRDisplayDevice*>& BaseDisplayDevice::getSubDisplays() const {
	return subDisplays;
}

void BaseDisplayDevice::addSubDisplay(VRDisplayDevice* display)
{
	subDisplays.push_back(display);
}

void BaseDisplayDevice::useDisplay(const MinVR::VRDisplayAction& action)
{
	useAllDisplays(action);
}

void BaseDisplayDevice::startRenderingAllDisplays(const MinVR::VRRenderer& renderer, VRRenderState& state)
{
	if (subDisplays.size() > 0)
	{
		for (int f = 0; f < subDisplays.size(); f++)
		{
			subDisplays[f]->startRendering(renderer, state);
		}
	}
	else
	{
		state.display = this;
		renderer.render(state);
	}
}

void BaseDisplayDevice::useAllDisplays(const MinVR::VRDisplayAction& action)
{
	if (subDisplays.size() > 0)
	{
		for (int f = 0; f < subDisplays.size(); f++)
		{
			subDisplays[f]->use(action);
		}
	}
	else
	{
		action.exec();
	}
}

std::string BaseDisplayDevice::getName() const {
	return name;
}

void BaseDisplayDevice::setName(std::string& name) {
	this->name = name;
}

MinVR::VRFrameController* BaseDisplayDevice::getFrameController() {
	MinVR::VRFrameController* controller = NULL;

	for (int f = 0; f < subDisplays.size(); f++)
	{
		controller = subDisplays[f]->getFrameController();

		if (controller != NULL)
		{
			return controller;
		}
	}

	return controller;
}

void BaseDisplayDevice::finishRenderingAllDisplays()
{
	for (int f = 0; f < subDisplays.size(); f++)
	{
		subDisplays[f]->finishRendering();
	}
}

} /* namespace MinVR */
