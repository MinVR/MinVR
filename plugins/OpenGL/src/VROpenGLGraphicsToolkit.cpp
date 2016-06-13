
#include "VROpenGLGraphicsToolkit.h"
#include "VROpenGLHeaders.h"

namespace MinVR {

VROpenGLGraphicsToolkit::VROpenGLGraphicsToolkit() {

}
	
VROpenGLGraphicsToolkit::~VROpenGLGraphicsToolkit() {

}

void VROpenGLGraphicsToolkit::clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void VROpenGLGraphicsToolkit::setDrawBuffer(VRDRAWBUFFER buffer) {
	if (buffer == VRDRAWBUFFER_BACK) {
		glDrawBuffer(GL_BACK);
	}
	else if (buffer == VRDRAWBUFFER_FRONT) {
		glDrawBuffer(GL_FRONT);
	}
	else if (buffer == VRDRAWBUFFER_BACKLEFT) {
		glDrawBuffer(GL_BACK_LEFT);
	}
	else if (buffer == VRDRAWBUFFER_FRONTLEFT) {
		glDrawBuffer(GL_FRONT_LEFT);
	}
	else if (buffer == VRDRAWBUFFER_BACKRIGHT) {
		glDrawBuffer(GL_BACK_RIGHT);
	}
	else if (buffer == VRDRAWBUFFER_FRONTRIGHT) {
		glDrawBuffer(GL_FRONT_RIGHT);
	}
}

void VROpenGLGraphicsToolkit::setViewport(VRRect rect) {
  glViewport(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void VROpenGLGraphicsToolkit::flushGraphics() {
	glFlush();
}

void VROpenGLGraphicsToolkit::finishGraphics() {
	glFinish();
}


VRGraphicsToolkit*
VROpenGLGraphicsToolkit::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
	VRGraphicsToolkit *gfxToolkit = new VROpenGLGraphicsToolkit();
	return gfxToolkit;
}


} // end namespace
