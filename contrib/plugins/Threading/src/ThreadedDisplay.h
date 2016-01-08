/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef THREADEDDISPLAY_H_
#define THREADEDDISPLAY_H_

#include "plugin/PluginFramework.h"
#include "display/VRDisplayDevice.h"
#include "RenderThread.h"
#include "Thread.h"

namespace MinVR {

class ThreadedDisplay: public VRDisplayDevice {
public:
	PLUGIN_API ThreadedDisplay();
	PLUGIN_API virtual ~ThreadedDisplay();

	PLUGIN_API void initialize();
	PLUGIN_API void useDisplay(const MinVR::VRDisplayAction& action);
	PLUGIN_API void finishRendering();

protected:
	PLUGIN_API void startRendering(const MinVR::VRRenderer& renderer, int x);

private:
	std::vector<RenderThread*> renderThreads;
	RenderThreadInfo threadInfo;
	int frame;
};

class ThreadedDisplayFactory : public SimpleVRDisplayFactory {
public:
	ThreadedDisplayFactory();
	virtual ~ThreadedDisplayFactory();

	PLUGIN_API VRDisplayDevice* createDisplay(const std::string type, const std::string name, VRDataIndex& config, VRDisplayDeviceFactory* factory);
};

} /* namespace MinVR */

#endif /* THREADEDDISPLAY_H_ */
