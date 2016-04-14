
#include "VROpenGLGraphicsToolkit.h"
#include "VROpenGLHeaders.h"

namespace MinVR {

VROpenGLGraphicsToolkit::VROpenGLGraphicsToolkit() {

}
	
VROpenGLGraphicsToolkit::~VROpenGLGraphicsToolkit() {

}

void VROpenGLGraphicsToolkit::clearScreen() {
	glClear(true, true, true);
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
	int xOffset = x + getParent()->getXOffset();
	int yOffset = y + getParent()->getYOffset();
	int viewportWidth = min(width, getParent()->getWidth()-x);
	int viewportHeight = min(height, getParent()->getHeight()-y);
	glViewport(xOffset, yOffset, viewportWidth, viewportHeight);
	glScissor(xOffset, yOffset, viewportWidth, viewportHeight);
}

void VROpenGLGraphicsToolkit::flushGraphics() {
	glFlush();
}

void VROpenGLGraphicsToolkit::finishGraphics() {
	glFinish();
}


VRGraphicsToolkit*
VROpenGLGraphicsToolkitFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) {
	std::string nameSpace = nameSpace + "/" + valName;

	std::string type = config->getValue("Type", nodeNameSpace);
	if (type != "VROpenGLGraphicsToolkit") {
		// This factory cannot create the type specified
		return NULL;
	}

	VRGraphicsToolkit *gfxToolkit = new VROpenGLGraphicsToolkit();
	return gfxToolkit;
}


} // end namespace

#endif