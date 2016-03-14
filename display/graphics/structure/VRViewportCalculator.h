/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRVIEWPORTCALCULATOR_H_
#define VRVIEWPORTCALCULATOR_H_

#include "display/VRRenderer.h"
#include "VRViewport.h"

namespace MinVR {

class VRViewportCalculator {
public:
	VRViewportCalculator(bool modifyTile = false);
	virtual ~VRViewportCalculator();

	void preRender(VRRenderer& renderer, const VRViewport& viewport);
	void postRender(VRRenderer& renderer);

private:
	bool m_modifyTile;
};

} /* namespace MinVR */

#endif /* VRVIEWPORTCALCULATOR_H_ */
