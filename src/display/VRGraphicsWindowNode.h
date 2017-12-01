/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRGRAPHICSWINDOWNODE_H_
#define VRGRAPHICSWINDOWNODE_H_

#include <string>

#include <display/VRDisplayNode.h>
#include <display/VRGraphicsToolkit.h>
#include <display/VRWindowToolkit.h>
#include <main/VRFactory.h>
#include <math/VRRect.h>

namespace MinVR {



/** Each VRGraphicsWindowNode has a VRWindowToolkit. 
 */
class VRGraphicsWindowNode : public VRDisplayNode {
public:
	VRGraphicsWindowNode(const std::string &name, VRGraphicsToolkit *gfxToolkit, VRWindowToolkit *winToolkit, const VRWindowSettings &settings);
	virtual ~VRGraphicsWindowNode();

	virtual std::string getType() const { return "VRGraphicsWindowNode"; }

	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);
	virtual void waitForRenderToComplete(VRDataIndex *renderState);
	virtual void displayFinishedRendering(VRDataIndex *renderState);

	static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

private:
	int _windowID;
	VRGraphicsToolkit *_gfxToolkit;
	VRWindowToolkit   *_winToolkit;
	VRWindowSettings   _settings;
	int _framebufferWidth;
	int _framebufferHeight;
};


} /* namespace MinVR */

#endif /* VRGRAPHICSWINDOWNODE_H_ */
