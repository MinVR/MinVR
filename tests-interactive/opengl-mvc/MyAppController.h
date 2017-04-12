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
#include <map>

class MyAppController : public VRApp {
public:
	MyAppController(int argc, char** argv, const std::string& configFile);
	virtual ~MyAppController();

	void onVREvent(const VREvent &event);
	void onVRRenderGraphicsContext(const VRGraphicsState &renderState);
	void onVRRenderGraphics(const VRGraphicsState &renderState);

private:
	MyAppModel model;
	std::map<int, MyAppView*> views;
};

#endif

