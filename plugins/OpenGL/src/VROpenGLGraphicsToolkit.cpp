
#include "VROpenGLGraphicsToolkit.h"
#include "VROpenGLHeaders.h"

namespace MinVR {

VROpenGLGraphicsToolkit::VROpenGLGraphicsToolkit() {

}
	
VROpenGLGraphicsToolkit::~VROpenGLGraphicsToolkit() {

}

void VROpenGLGraphicsToolkit::clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	glEnable(GL_SCISSOR_TEST);
	glViewport(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
  	glScissor(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void VROpenGLGraphicsToolkit::flushGraphics() {
	glFlush();
}

void VROpenGLGraphicsToolkit::finishGraphics() {
	glFinish();
}


VRGraphicsToolkit*
VROpenGLGraphicsToolkitFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) {
	std::string tkNameSpace = config->validateNameSpace(nameSpace + valName);

	std::string type = config->getValue("Type", tkNameSpace);
	if (type != "VROpenGLGraphicsToolkit") {
		// This factory cannot create the type specified
		return NULL;
	}

	VRGraphicsToolkit *gfxToolkit = new VROpenGLGraphicsToolkit();
	return gfxToolkit;
}


} // end namespace
