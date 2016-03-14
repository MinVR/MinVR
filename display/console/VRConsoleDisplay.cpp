/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/console/VRConsoleDisplay.h>

namespace MinVR {

VRConsoleDisplay::VRConsoleDisplay(std::ostream *stream) : m_stream(stream) {
	// TODO Auto-generated constructor stub

}

VRConsoleDisplay::~VRConsoleDisplay() {
	// TODO Auto-generated destructor stub
}

void VRConsoleDisplay::render(VRRenderer& renderer) {

}

void VRConsoleDisplay::startRender(VRRenderer& renderer) {
	renderer.pushState();
	renderer.getState().setValue("isConsole", 1);
	renderer.render();
	renderer.popState();
}

void VRConsoleDisplay::waitForRenderComplete() {
	*m_stream << "render complete." << std::endl;
}

void VRConsoleDisplay::synchronize() {
	std::flush(*m_stream);
	*m_stream << "flush." << std::endl;
}

} /* namespace MinVR */


