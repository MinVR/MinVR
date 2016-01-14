/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "display/VRDisplayDevice.h"

void VRDisplayDevice::initialize()
{
}

bool VRDisplayDevice::renderFrame(MinVR::VRDisplayFrameAction& frameAction)
{
	MinVR::VRFrameController* controller = getFrameController();
	if (controller != NULL)
	{
		return controller->renderFrame(frameAction);
	}
	else
	{
		return frameAction.exec();
	}
}

void VRDisplayDevice::use(const MinVR::VRDisplayAction& action)
{
	useDisplay(action);
}

void VRDisplayDevice::startRendering(const MinVR::VRRenderer& renderer)
{
	VRRenderState state;
	startRendering(renderer, state);
}

void VRDisplayDevice::render(const MinVR::VRRenderer& renderer)
{
	startRendering(renderer);
	finishRendering();
}

void VRDisplayDevice::render(void (*method)(VRRenderState&))
{
	render(MinVR::VRRendererFunctor(method));
}

void VRDisplayDevice::use(void (*method)())
{
	use(MinVR::VRDisplayActionFunctor(method));
}

bool VRDisplayDevice::renderFrame(bool (*method)())
{
	MinVR::VRDisplayFrameActionFunctor functor(method);
	return renderFrame(functor);
}

void VRDisplayDevice::startRendering(void (*method)(VRRenderState&))
{
	startRendering(MinVR::VRRendererFunctor(method));
}

void VRDisplayDevice::startRendering(VRDisplayDevice* &display, const MinVR::VRRenderer& renderer, VRRenderState& renderState)
{
	display->startRendering(renderer, renderState);
}
