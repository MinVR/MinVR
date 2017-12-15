/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "main/VRMain.h"
#include <main/VREventHandler.h>
#include <main/VRRenderHandler.h>
#include <plugin/VRPlugin.h>

using namespace MinVR;

// Event and Render python callback functions
extern "C" {
	PLUGIN_API typedef int (*eventcallback_type)(const char* eventName, void* eventData);
	PLUGIN_API typedef int (*rendercallback_type)(void* renderState);
}

// Event handler callback wrapper
class VRPythonEventCallbackHandler : public VREventHandler {
public:
	PLUGIN_API VRPythonEventCallbackHandler(eventcallback_type eventCallback) : eventCallback(eventCallback) {}
	PLUGIN_API virtual ~VRPythonEventCallbackHandler() {}

	PLUGIN_API virtual void onVREvent(const VRDataIndex &event) {
		eventCallback(event.getName().c_str(), (void*)&event);
	}

private:
	eventcallback_type eventCallback;
};

// Render handler callback wrapper
class VRPythonRenderCallbackHandler : public VRRenderHandler {
public:
	PLUGIN_API VRPythonRenderCallbackHandler(rendercallback_type renderCallback, rendercallback_type renderContextCallback) : renderCallback(renderCallback), renderContextCallback(renderContextCallback) {}
	PLUGIN_API virtual ~VRPythonRenderCallbackHandler() {}
    
	PLUGIN_API virtual void onVRRenderScene(const VRDataIndex &renderState) {
		renderCallback((void*)&renderState);
	}
	PLUGIN_API virtual void onVRRenderContext(const VRDataIndex &renderState) {
		renderContextCallback((void*)&renderState);
	}

private:
	rendercallback_type renderCallback;
	rendercallback_type renderContextCallback;
};

// Python hooks
extern "C" {

	// Create VRMain
	PLUGIN_API VRMain* VRMain_init(char* searchPath, int argc, char** argv) {
		VRMain* vrmain = new VRMain();
		vrmain->addPluginSearchPath(std::string(searchPath));
		vrmain->initialize(argc, argv);
		std::cout << "Plugin directory: " << std::string(searchPath) << std::endl;
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
	PLUGIN_API VRRenderHandler* VRMain_registerRenderCallback(VRMain* vrmain, rendercallback_type renderCallback, rendercallback_type renderContextCallback) {
		VRRenderHandler* handler = new VRPythonRenderCallbackHandler(renderCallback, renderContextCallback);
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
