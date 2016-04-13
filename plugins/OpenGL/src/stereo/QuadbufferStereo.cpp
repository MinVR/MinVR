/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <stereo/QuadbufferStereo.h>
#include "OpenGLGraphics.h"

namespace MinVR {

QuadbufferStereo::QuadbufferStereo() {
}

QuadbufferStereo::~QuadbufferStereo() {
}

void QuadbufferStereo::startRendering(const MinVR::VRRenderer& renderer, VRRenderState& state) {
	glDrawBuffer(GL_BACK_LEFT);
	startRenderingAllDisplays(renderer, state);
	glDrawBuffer(GL_BACK_RIGHT);
	startRenderingAllDisplays(renderer, state);
}

void QuadbufferStereo::finishRendering() {
	glDrawBuffer(GL_BACK);
}

} /* namespace MinVR */
