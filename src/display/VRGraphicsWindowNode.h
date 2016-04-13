/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRGRAPHICSWINDOWNODE_H_
#define VRGRAPHICSWINDOWNODE_H_

#include "VRDisplayNode.h"
#include "VRWindwoToolkit.h"
#include "math/VRRect.h"

namespace MinVR {



/** Each VRGraphicsWindowNode has a VRWindowToolkit. 
 */
class VRGraphicsWindowNode : public VRDisplayNode {
public:
	VRGraphicsWindowNode(const std::string &name, VRGraphicsToolkit, *gfxToolkit, VRWindowToolkit *winToolkit, const VRWindowSettings &settings);
	virtual ~VRGraphicsWindowNode();

	virtual std::string getType() { return "VRGraphicsWindowNode"; }

	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);
	virtual void waitForRenderToComplete(VRDataIndex *renderState);
	virtual void displayFinishedRendering(VRDataIndex *renderState);

private:
	int _windowID;
	VRGraphicsToolkit *_gfxToolkit;
	VRWindowToolkit   *_winToolkit;
	VRWindowSettings   _settings;
};


class VRGraphicsWindowNodeFactory : public VRDisplayNodeFactory {
public:
	virtual ~VRGraphicsWindowNodeFactory() {}

	VRDisplayNode* create(VRMain *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace);
};


} /* namespace MinVR */

#endif /* VRGRAPHICSWINDOWNODE_H_ */
