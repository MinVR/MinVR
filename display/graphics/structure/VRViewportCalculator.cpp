/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/graphics/structure/VRViewportCalculator.h>
#include "VRTile.h"

namespace MinVR {

VRViewportCalculator::VRViewportCalculator(bool modifyTile) : m_modifyTile(modifyTile) {
}

VRViewportCalculator::~VRViewportCalculator() {
}

VRViewport VRViewportCalculator::calculate(VRRenderState& state,
		const VRViewport& viewport) {

	const VRViewport* currentViewport = &viewport;

	VRViewport oldViewport;
	VRViewport modifiedViewport;
	if (state.readValue("viewport", oldViewport))
	{
		modifiedViewport = oldViewport.generateChild(viewport);
		currentViewport = &modifiedViewport;
	}

	if (m_modifyTile)
	{
		VRTile tile;
		if (state.readValue("tile", tile))
		{
			tile = tile.modifyWithViewport(oldViewport, *currentViewport);
			state.writeValue("tile", tile);
		}
	}

	state.writeValue("viewport", *currentViewport);

	return *currentViewport;
}

} /* namespace MinVR */
