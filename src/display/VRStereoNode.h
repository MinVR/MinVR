/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRSTEREONODE_H_
#define VRSTEREONODE_H_

#include <display/VRDisplayNode.h>
#include <display/VRGraphicsToolkit.h>
#include <main/VRFactory.h>

namespace MinVR {



class VRStereoNode : public VRDisplayNode {
public:
	enum VRStereoFormat {
		VRSTEREOFORMAT_MONO = 0,
		VRSTEREOFORMAT_QUADBUFFERED = 1,
		VRSTEREOFORMAT_SIDEBYSIDE = 2,
		VRSTEREOFORMAT_COLUMNINTERLACED = 3
		// ..
	};

	VRStereoNode(const std::string &name, VRGraphicsToolkit *gfxToolkit, VRStereoFormat format);
	virtual ~VRStereoNode();

	virtual std::string getType() { return "VRStereoNode"; }

	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

protected:

	VRGraphicsToolkit *_gfxToolkit;
	VRStereoFormat _format;
};



class VRStereoNodeFactory : public VRDisplayNodeFactory {
public:
	VRStereoNodeFactory():VRDisplayNodeFactory("VRStereoNode"){}

	virtual ~VRStereoNodeFactory(){}

	VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);
};

} /* namespace MinVR */

#endif /* VRSTEREOFORMATTER_H_ */
