/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef FRCONSOLEDISPLAY_H_
#define VRCONSOLEDISPLAY_H_

#include "display/VRDisplay.h"

namespace MinVR {

class VRConsoleDisplay : public VRDisplay {
public:
	VRConsoleDisplay();
	virtual ~VRConsoleDisplay();

	void render(VRRenderer& renderer);
};

} /* namespace MinVR */

#endif /* CONSOLEDISPLAY_H_ */
