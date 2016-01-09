/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef DATAINDEXWRAPPERDISPLAY_H_
#define DATAINDEXWRAPPERDISPLAY_H_

#include "display/concrete/BaseDisplayDevice.h"

namespace MinVR {

class DataIndexWrapperDisplay : public BaseDisplayDevice {
public:
	DataIndexWrapperDisplay(VRDisplayDevice* device, VRDataIndex* index);
	virtual ~DataIndexWrapperDisplay();

	void finishRendering();

protected:
	void startRendering(const VRRenderer& renderer, VRRenderState& state);

private:
	VRDataIndex* index;
};

} /* namespace MinVR */

#endif /* DATAINDEXWRAPPERDISPLAY_H_ */
