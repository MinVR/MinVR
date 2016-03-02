/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

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

void VRDisplayDevice::render(const MinVR::VRRenderer& renderer)
{
	MinVR::VRRenderState rs;
	startRendering(renderer, rs);
	finishRendering();
}