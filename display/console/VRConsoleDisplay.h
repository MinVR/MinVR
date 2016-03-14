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
#include "display/synchronization/VRSynchronizedDisplay.h"
#include <iostream>

namespace MinVR {

class VRConsoleDisplay : public VRDisplay, public VRSynchronizedDisplay {
public:
	VRConsoleDisplay(std::ostream *stream = &std::cout);
	virtual ~VRConsoleDisplay();

	void render(VRRenderer& renderer);
	void startRender(VRRenderer& renderer);
	void waitForRenderComplete();
	void synchronize();

private:
	std::ostream* m_stream;
};

} /* namespace MinVR */

#endif /* CONSOLEDISPLAY_H_ */
