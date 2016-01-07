/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <stereo/SideBySideStereo.h>
#include "OpenGLGraphics.h"

namespace MinVR {

SideBySideStereo::SideBySideStereo() {
}

SideBySideStereo::~SideBySideStereo() {
}

void SideBySideStereo::finishRendering() {
	finishRenderingAllDisplays();
}

int SideBySideStereo::getXOffset() {
	return x;
}

int SideBySideStereo::getYOffset() {
	return y;
}

int SideBySideStereo::getWidth() {
	return width;
}

int SideBySideStereo::getHeight() {
	return height;
}

void SideBySideStereo::startRendering(const MinVR::VRRenderer& renderer,
		int t) {
	glEnable(GL_SCISSOR_TEST);
	glDrawBuffer(GL_BACK);

	x = getParent()->getXOffset();
	y = getParent()->getYOffset();
	width = getParent()->getWidth()/2;
	height = getParent()->getHeight();

	glViewport(x,y,width,height);
	glScissor(x,y,width,height);
	startRenderingAllDisplays(renderer, t);

	x = x + width;

	glViewport(x,y,width,height);
	glScissor(x,y,width,height);
	startRenderingAllDisplays(renderer, t);
}

} /* namespace MinVR */

