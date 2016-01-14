/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GLUTWINDOW_H_
#define GLUTWINDOW_H_

#include "plugin/PluginFramework.h"
#include "display/concrete/BaseDisplayDevice.h"
#include "display/concrete/BaseDisplayFactory.h"
#include "display/VRFrameController.h"

namespace MinVR {

class GLUTWindow : public BaseDisplayDevice {
public:
	PLUGIN_API GLUTWindow(int x, int y, int width, int height);
	PLUGIN_API virtual ~GLUTWindow();

	PLUGIN_API int getWidth();
	PLUGIN_API int getHeight();
	PLUGIN_API bool allowThreading() { return false; }

	PLUGIN_API void initialize();
	PLUGIN_API void finishRendering();
	PLUGIN_API void useDisplay(const MinVR::VRDisplayAction& action);

	PLUGIN_API MinVR::VRFrameController* getFrameController();

protected:
	PLUGIN_API void startRendering(const MinVR::VRRenderer& renderer, VRRenderState& state);

private:
	int x, y, width, height;
};

class GLUTFrameController : public VRFrameController {
public:
	PLUGIN_API GLUTFrameController();
	PLUGIN_API ~GLUTFrameController();

	PLUGIN_API bool renderFrame(VRDisplayFrameAction& frameAction);
};

class GLUTWindowFactory : public BaseDisplayFactory {
public:
	PLUGIN_API GLUTWindowFactory();
	PLUGIN_API virtual ~GLUTWindowFactory();

	PLUGIN_API VRDisplayDevice* createDisplay(const std::string type, const std::string name, VRDataIndex& config, VRDisplayDeviceFactory* factory);
};

} /* namespace MinVR */

#endif /* GLUTWINDOW_H_ */
