/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/graphics/structure/VRViewportFormatter.h>

namespace MinVR {

VRViewportFormatter::VRViewportFormatter() {
	// TODO Auto-generated constructor stub

}

VRViewportFormatter::~VRViewportFormatter() {
	// TODO Auto-generated destructor stub
}

void VRViewportFormatter::preRender(VRRenderer& renderer,
		const VRViewport& viewport) {

	const VRViewport* currentViewport = &viewport;

	VRViewport modifiedViewport;
	if (renderer.getState().getValue("viewport", modifiedViewport))
	{
		modifiedViewport = modifiedViewport.generateChild(viewport);
		currentViewport = &modifiedViewport;
	}

	renderer.pushState();
	VRDataIndex& state = renderer.getState().getDataIndex();
	renderer.getState().setValue("viewport", *currentViewport);
}

void VRViewportFormatter::postRender(VRRenderer& renderer) {
	renderer.popState();
}

} /* namespace MinVR */
