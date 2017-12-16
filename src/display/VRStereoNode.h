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


/** Looks for a HeadMatrix in the RenderState and turns it into a CameraMatrix
    based on the current stereo format and value for EyeSeparation.  Then, calls
    the rest of the display graph one or two times (if stereo is enabled).  Also 
    sets an entry in the RenderState for the current Eye being rendered.
 */
class VRStereoNode : public VRDisplayNode {
public:
	enum VRStereoFormat {
		VRSTEREOFORMAT_MONO = 0,
		VRSTEREOFORMAT_QUADBUFFERED = 1,
		VRSTEREOFORMAT_SIDEBYSIDE = 2,
		VRSTEREOFORMAT_COLUMNINTERLACED = 3
		// ..
	};

	enum VREyePosition
	{
		Cyclops = 0,
		Left = 1,
		Right = 2
	};
	VRStereoNode(const std::string &name, float interOcularDist, VRGraphicsToolkit *gfxToolkit, VRStereoFormat format);
	virtual ~VRStereoNode();

	virtual std::string getType() const { return "VRStereoNode"; }

	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);
	virtual void createChildren(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

	static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

protected:
	void renderOneEye(VRDataIndex *renderState, VRRenderHandler *renderHandler, VREyePosition eye);
	void setCameraMatrix(VRDataIndex *renderState, VREyePosition eye);

	VRGraphicsToolkit *_gfxToolkit;
	VRStereoFormat _format;
	float     _iod;
};


} /* namespace MinVR */

#endif /* VRSTEREOFORMATTER_H_ */
