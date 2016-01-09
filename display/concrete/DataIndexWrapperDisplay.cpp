/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/concrete/DataIndexWrapperDisplay.h>

namespace MinVR {

DataIndexWrapperDisplay::DataIndexWrapperDisplay(VRDisplayDevice* device, VRDataIndex* index) : index(index) {
	addSubDisplay(device);
}

DataIndexWrapperDisplay::~DataIndexWrapperDisplay() {
	// TODO Auto-generated destructor stub
}

void DataIndexWrapperDisplay::finishRendering() {
	finishRenderingAllDisplays();
}

void DataIndexWrapperDisplay::startRendering(const VRRenderer& renderer,
		VRRenderState& state) {
	state.dataIndex = this->index;
	startRenderingAllDisplays(renderer, state);
}

} /* namespace MinVR */
