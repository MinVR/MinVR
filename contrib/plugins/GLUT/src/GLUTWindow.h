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
#include "event/VRInputDevice.h"
#include "config/VRDataIndex.h"
#include <vector>

namespace MinVR {

class GLUTWindow : public BaseDisplayDevice, public VRInputDevice {
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

	PLUGIN_API void appendNewInputEventsSinceLastCall(VRDataQueue& queue);

protected:
	PLUGIN_API void startRendering(const MinVR::VRRenderer& renderer, VRRenderState& state);

private:
	static void keyboardInput(std::string key, int x, int y, std::string action);
	static void keyboardInput(unsigned char c, int x, int y);
	static void keyboardUpInput(unsigned char c, int x, int y);
	static void keyboardSpecialInput(int key, int x, int y);
	static void keyboardSpecialUpInput(int key, int x, int y);
	static std::string determineSpecialKey(int key);

	int x, y, width, height;
};

class GLUTFrameController : public VRFrameController {
public:
	PLUGIN_API GLUTFrameController();
	PLUGIN_API ~GLUTFrameController();

	PLUGIN_API bool renderFrame(VRDisplayFrameAction& frameAction);

private:
	static void windowLoop();
};

class GLUTWindowFactory : public BaseDisplayFactory, public VRInputDeviceFactory {
public:
	PLUGIN_API GLUTWindowFactory();
	PLUGIN_API virtual ~GLUTWindowFactory();

	PLUGIN_API VRDisplayDevice* createDisplay(const std::string type, const std::string name, VRDataIndex& config, VRDisplayDeviceFactory* factory);
	PLUGIN_API std::vector<VRInputDevice*> create(VRDataIndex& dataIndex);
};

} /* namespace MinVR */

#endif /* GLUTWINDOW_H_ */
