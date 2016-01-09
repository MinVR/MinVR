/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef COMPOSITEDISPLAY_H_
#define COMPOSITEDISPLAY_H_

#include "display/concrete/BaseDisplayDevice.h"

namespace MinVR {

class CompositeDisplay : public BaseDisplayDevice {
public:
	CompositeDisplay(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory);
	virtual ~CompositeDisplay();

	void finishRendering();

protected:
	void startRendering(const MinVR::VRRenderer& renderer, VRRenderState& state);
};

} /* namespace MinVR */

#endif /* COMPOSITEDISPLAY_H_ */
