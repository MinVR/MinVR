/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "MyAppController.h"
#include <cmath>

// Just included for some simple Matrix math used below
// This is not required for use of MinVR in general

MyAppController::MyAppController(int argc, char** argv) : VRApp(argc, argv) {
}

MyAppController::~MyAppController() {
	for (std::map<int, MyAppView*>::iterator it = views.begin(); it != views.end(); it++) {
		delete it->second;
	}

	for (std::map<int, MyAppSharedContext*>::iterator it = sharedContexts.begin(); it != sharedContexts.end(); it++) {
		delete it->second;
	}
}

void MyAppController::onVREvent(const VREvent &event) {

	event.print();

	// Set time since application began
	if (event.getName() == "FrameStart") {
		float time = event.getDataAsFloat("ElapsedSeconds");
		// Calculate model matrix based on time
		float* m = &model.modelMatrix[0];

		const float cosTheta = std::cos(time);
		const float sinTheta = std::sin(time);
		m[0]=1.0; m[4]=0.0;  m[8]=0.0; m[12]=0.0;
		m[1]=0.0; m[5]=cosTheta;  m[9]=-sinTheta; m[13]=0.0;
		m[2]=0.0; m[6]=sinTheta; m[10]=cosTheta; m[14]=0.0;
		m[3]=0.0; m[7]=0.0; m[11]=0.0; m[15]=1.0;
		return;
	}

	// Quit if the escape button is pressed
	if (event.getName() == "KbdEsc_Down") {
		shutdown();
	}
}

/// onVRRenderContext is the override which allows users to setup context specific
/// variables like VBO's, VAO's, textures, framebuffers, and shader programs.
void MyAppController::onVRRenderGraphicsContext(const VRGraphicsState &renderState) {
	int windowId = renderState.getWindowId();

	// If this is the inital call, initialize context variables
	if (renderState.isInitialRenderCall()) {
		sharedContexts[windowId] = new MyAppSharedContext(model, renderState);
		views[windowId] = new MyAppView(model, *sharedContexts[windowId], renderState);
	}
	else {
		views[windowId]->update(renderState);
	}

	// Destroy context items if the program is no longer running
	if (!isRunning()) {
		std::map<int, MyAppView*>::iterator it = views.find(windowId);
		delete it->second;
		views.erase(it);
		return;
	}
}

/// onVRRenderScene will run draw calls on each viewport inside a context.
void MyAppController::onVRRenderGraphics(const VRGraphicsState &renderState) {
	// Only draw if the application is still running.
	if (isRunning()) {
		int windowId = renderState.getWindowId();

		views[windowId]->render(renderState);
	}
}
