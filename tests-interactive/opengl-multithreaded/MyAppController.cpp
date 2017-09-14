/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "MyAppController.h"
#include <cmath>

MyAppController::MyAppController(int argc, char** argv) : VRApp(argc, argv), frame(0) {
}

MyAppController::~MyAppController() {
}

void MyAppController::onAnalogChange(const VRAnalogEvent &state) {

	//event.print();

	// Set time since application began
	if (state.getName() == "FrameStart") {
		frame++;
        float time = state.getValue();
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
}

void MyAppController::onButtonDown(const MinVR::VRButtonEvent &state) {
	// Quit if the escape button is pressed
	if (state.getName() == "KbdEsc_Down") {
		shutdown();
	}
}

/// onVRRenderContext is the override which allows users to setup context specific
/// variables like VBO's, VAO's, textures, framebuffers, and shader programs.
void MyAppController::onRenderGraphicsContext(const VRGraphicsState &renderState) {
	int windowId = renderState.getWindowId();
	int sharedContextId = renderState.getSharedContextId();

	// If this is the inital call, initialize context variables
	if (renderState.isInitialRenderCall()) {
		// Need to lock here so that we can add the context objects one by one
		std::unique_lock<std::mutex> lock(mutex);

		// If a shared context Id exists, create one per shared context, otherwise one per window
		MyAppSharedContext* context = NULL;
		if (sharedContextId < 0) {
			// Create a shared context per window if this window doesn't have a shared context
			context = new MyAppSharedContext(model, renderState);
			normalContexts[windowId] = context;
		}
		else {
			// Use shared context if it already exists, otherwise create one
			std::map<int, MyAppSharedContext*>::iterator it = sharedContexts.find(sharedContextId);
			if (it == sharedContexts.end()) {
				context = new MyAppSharedContext(model, renderState);
				sharedContexts[sharedContextId] = context;
				sharedContextCount[sharedContextId] = 1;
			}
			else {
				context = it->second;
				sharedContextCount[sharedContextId]++;
			}
		}

		// Create one view per window and use shared context
		views[windowId] = new MyAppView(model, *context, renderState);
		lock.unlock();
	}
	else {
		if (sharedContextId < 0) {
			// Update window shared context if no shared context exists
			normalContexts[windowId]->update(renderState, frame);
		}
		else {
			// Update shared context if it exists
			sharedContexts[sharedContextId]->update(renderState, frame);
		}

		// Update the view for the window
		views[windowId]->update(renderState);
	}

	// Destroy context items if the program is no longer running
    /** 
     DAN K. to DAN O. -- not sure about isRunning()????
     
	if (!isRunning()) {
		// Need to lock for deleting
		std::unique_lock<std::mutex> lock(mutex);

		// Delete view
		{
			std::map<int, MyAppView*>::iterator it = views.find(windowId);
			delete it->second;
			views.erase(it);
		}

		// Delete shared contexts
		if (sharedContextId < 0) {
			std::map<int, MyAppSharedContext*>::iterator it = normalContexts.find(windowId);
			delete it->second;
			normalContexts.erase(it);
		}
		else {
			sharedContextCount[sharedContextId]--;
			if (sharedContextCount[sharedContextId] == 0) {
				std::map<int, MyAppSharedContext*>::iterator it = sharedContexts.find(sharedContextId);
				delete it->second;
				sharedContexts.erase(it);
			}
		}

		lock.unlock();

		return;
	}
     */
}

/// onVRRenderScene will run draw calls on each viewport inside a context.
void MyAppController::onRenderGraphicsScene(const VRGraphicsState &renderState) {
	// Only draw if the application is still running.
    
    /**
     DAN K. to DAN O. -- not sure about isRunning()????

	if (isRunning()) {*/
		int windowId = renderState.getWindowId();

		// Render on view
		views[windowId]->render(renderState);
    //}
}
