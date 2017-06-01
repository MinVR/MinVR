/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef MYAPPCONTROLLER_H_
#define MYAPPCONTROLLER_H_

// MinVR header
#include <api/MinVR.h>
using namespace MinVR;

#include "MyAppModel.h"
#include "MyAppView.h"
#include "MyAppSharedContext.h"
#include <map>
#include <mutex>

/** MyAppController acts as the main part of the application by handling events, creating
 * and managing views, as well as managing shared contexts.
 */
class MyAppController : public VRApp {
public:
	MyAppController(int argc, char** argv);
	virtual ~MyAppController();

	/// Handles VR Events
	void onVREvent(const VREvent &event);
	/// Handles and updates Graphics Context
	void onVRRenderGraphicsContext(const VRGraphicsState &renderState);
	/// Handles Graphics rendering
	void onVRRenderGraphics(const VRGraphicsState &renderState);

private:
	MyAppModel model;
	std::map<int, MyAppView*> views;
	std::map<int, MyAppSharedContext*> sharedContexts;
	std::map<int, int> sharedContextCount;
	std::map<int, MyAppSharedContext*> normalContexts;
	std::mutex mutex;
	int frame;
};

#endif

