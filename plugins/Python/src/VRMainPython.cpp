/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "main/VRMain.h"
#include <plugin/VRPlugin.h>

using namespace MinVR;

// Event and Render python callback functions
extern "C" {
	PLUGIN_API typedef int (*eventcallback_type)(const char* eventName);
	PLUGIN_API typedef int (*rendercallback_type)(void* renderState);
}

// Event handler callback wrapper
class VRPythonEventCallbackHandler : public VREventHandler {
public:
	PLUGIN_API VRPythonEventCallbackHandler(eventcallback_type eventCallback) : eventCallback(eventCallback) {}
	PLUGIN_API virtual ~VRPythonEventCallbackHandler() {}

	PLUGIN_API void onVREvent(const std::string &eventName, VRDataIndex *eventData) {
		eventCallback(eventName.c_str());
	}

private:
	eventcallback_type eventCallback;
};

// Render handler callback wrapper
class VRPythonRenderCallbackHandler : public VRRenderHandler {
public:
	PLUGIN_API VRPythonRenderCallbackHandler(rendercallback_type renderCallback) : renderCallback(renderCallback) {}
	PLUGIN_API virtual ~VRPythonRenderCallbackHandler() {}
	PLUGIN_API virtual void onVRRenderScene(VRDataIndex *renderState, VRDisplayNode *callingNode) {
		renderCallback(renderState);
	}
	PLUGIN_API virtual void onVRRenderContext(VRDataIndex *renderState, VRDisplayNode *callingNode) {}

private:
	rendercallback_type renderCallback;
};

// Python hooks
extern "C" {

	// Create VRMain
	PLUGIN_API VRMain* VRMain_init(char* config) {
		VRMain* vrmain = new VRMain();
		char** input = new char*[2];
		input[1] = config;
		vrmain->initialize(2, input);
		delete[] input;
		return vrmain;
	}

	// Shutdown VRMain
	PLUGIN_API void VRMain_shutdown(VRMain* vrmain, VREventHandler* eventHandler, VRRenderHandler* renderHandler) {
		vrmain->shutdown();
		delete eventHandler;
		delete renderHandler;
		delete vrmain;
	}

	// Register event callback
	PLUGIN_API VREventHandler* VRMain_registerEventCallback(VRMain* vrmain, eventcallback_type eventCallback) {
		VREventHandler* handler = new VRPythonEventCallbackHandler(eventCallback);
		vrmain->addEventHandler(handler);
		return handler;
	}

	// Register render callback
	PLUGIN_API VRRenderHandler* VRMain_registerRenderCallback(VRMain* vrmain, rendercallback_type renderCallback) {
		VRRenderHandler* handler = new VRPythonRenderCallbackHandler(renderCallback);
		vrmain->addRenderHandler(handler);
		return handler;
	}

	// Allow python specific plugins which calls the python modules specified
	PLUGIN_API bool setPluginList(VRMain* vrmain, char* pluginList) {
		if (vrmain->getConfig()->exists("PythonPlugins","/")) {
			std::string py = vrmain->getConfig()->getValue("PythonPlugins","/");
			strcpy(pluginList, py.c_str());

			return true;
		}

		return false;
	}

	// Runs main loop
	PLUGIN_API void VRMain_mainloop(VRMain* vrmain) {
		vrmain->mainloop();
	}
}
