/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "MyAppController.h"

// Just included for some simple Matrix math used below
// This is not required for use of MinVR in general
#include <math/VRMath.h>

MyAppController::MyAppController(int argc, char** argv, const std::string& configFile) : VRApp(argc, argv, configFile) {
}

MyAppController::~MyAppController() {
}

void MyAppController::onVREvent(const VREvent &event) {

	event.print();

	// Set time since application began
	if (event.getName() == "FrameStart") {
		float time = event.getDataAsFloat("ElapsedSeconds");
		// Calculate model matrix based on time
		VRMatrix4 modelM = VRMatrix4::rotationX(0.5*time);
		modelM = modelM * VRMatrix4::rotationY(0.5*time);
		for (int f = 0; f < 16; f++) {
			model.modelMatrix[f] = modelM.getArray()[f];
		}
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
	// If this is the inital call, initialize context variables
	if (renderState.isInitialRenderCall()) {
		view = new MyAppView(model, renderState);
	}
	else {
		view->update(renderState);
	}

	// Destroy context items if the program is no longer running
	if (!isRunning()) {
		delete view;
		return;
	}
}

/// onVRRenderScene will run draw calls on each viewport inside a context.
void MyAppController::onVRRenderGraphics(const VRGraphicsState &renderState) {
	// Only draw if the application is still running.
	if (isRunning()) {
		// clear screen
		view->render(renderState);
	}
}