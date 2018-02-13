
#include "VROpenGLGraphicsToolkit.h"
#include "VROpenGLHeaders.h"

namespace MinVR {

VROpenGLGraphicsToolkit::VROpenGLGraphicsToolkit() {

}
	
VROpenGLGraphicsToolkit::~VROpenGLGraphicsToolkit() {

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

void VROpenGLGraphicsToolkit::setSubWindow(VRRect rect) {
	glEnable(GL_SCISSOR_TEST);
	glViewport((GLint)rect.getX(), (GLint)rect.getY(), (GLsizei)rect.getWidth(), (GLsizei)rect.getHeight());
	glScissor((GLint)rect.getX(), (GLint)rect.getY(), (GLsizei)rect.getWidth(), (GLsizei)rect.getHeight());
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
