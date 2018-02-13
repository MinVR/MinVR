/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Ben Knorlein
 */

#include <GL/freeglut.h>
#include "VRFreeGLUTWindowToolkit.h"
#include <main/VRMainInterface.h>
#include <iostream>

namespace MinVR {


static void error_callback(const char *fmt, va_list ap)
{
    std::cerr << "FREEGLUT - ERROR : " << fmt << " - " << ap << std::endl;
}

static void warning_callback(const char *fmt, va_list ap)
{
    std::cerr << "FREEGLUT - WARNING : " << fmt << " - " << ap << std::endl;
}

VRFreeGLUTWindowToolkit::VRFreeGLUTWindowToolkit(VRMainInterface *vrMain) : _vrMain(vrMain), _inputDev(NULL) {

	glutInitErrorFunc(error_callback);
	glutInitWarningFunc(warning_callback);
	
	int argc = vrMain->getArgc();
	glutInit(&argc, vrMain->getArgv());
	
	_inputDev = new VRFreeGLUTInputDevice();
}

VRFreeGLUTWindowToolkit::~VRFreeGLUTWindowToolkit() {
    for (std::vector<int>::iterator it = _windows.begin(); it != _windows.end(); ++it){
			glutDestroyWindow(*it);
	}
}


int
VRFreeGLUTWindowToolkit::createWindow(VRWindowSettings settings) {
    unsigned int mode = GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL;
	
	if (settings.quadBuffered) {
  		mode = mode | GLUT_STEREO;
    } 
	
	glutInitDisplayMode(mode);
	glutInitWindowPosition(settings.xpos,settings.ypos);
	glutInitWindowSize(settings.width,settings.height);
	int windowID = glutCreateWindow(settings.caption.c_str());
	if(settings.border == 0) glutFullScreen();
	
	_inputDev->addWindow(windowID);
	
	if (_windows.size() == 0) {
		// if this is the first window created, then register the virtual input device
		// with VRMain so that VRMain will start polling GLFW for input events
		_vrMain->addInputDevice(_inputDev);
	}
	
	_windows.push_back(windowID);
	
	return (int)_windows.size()-1;
}

void 
VRFreeGLUTWindowToolkit::destroyWindow(int windowID) {
	glutDestroyWindow(_windows[windowID]);
}

void 
VRFreeGLUTWindowToolkit::makeWindowCurrent(int windowID) {
	glutSetWindow(_windows[windowID]);
}

void 
VRFreeGLUTWindowToolkit::swapBuffers(int windowID) {
	glutSetWindow(_windows[windowID]);
	glutSwapBuffers();
	
}

VRFreeGLUTWindowToolkit*
VRFreeGLUTWindowToolkit::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
	return new VRFreeGLUTWindowToolkit(vrMain);
}


} /* namespace MinVR */

