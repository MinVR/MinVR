/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */
#include <GL/freeglut.h>

#include <VRFreeGLUTInputDevice.h>
#include <ctime>
#include <cctype>

#include <api/VRAnalogEvent.h>
#include <api/VRButtonEvent.h>
#include <api/VRCursorEvent.h>


namespace MinVR {

static VRFreeGLUTInputDevice* device;

std::string getKeyName(int key, bool isSpecial);

static void freeglut_key_callback(unsigned char key, int x, int y) {
    device->keyCallback(key, x, y,true);
}

static void freeglut_key_callback_up(unsigned char key, int x, int y) {
    device->keyCallback(key, x, y,false);
}

static void freeglut_specialkey_callback(int key, int x, int y) {
    device->keyCallback(key, x, y,true,true);
}

static void freeglut_specialkey_callback_up(int key, int x, int y) {
    device->keyCallback(key, x, y,false,true);
}

static void freeglut_mousemove_callback(int xpos, int ypos) {
    device->cursorPositionCallback(xpos,ypos, true);
}

static void freeglut_mousemove_passive_callback(int xpos, int ypos) {
    device->cursorPositionCallback(xpos,ypos, false);
}

static void freeglut_mouse_callback(int button, int state, int x, int y){
	device->mouseButtonCallback(button,state, x, y);
}

void freeglut_mousewheel_callback(int wheel, int direction, int x, int y) {
	device->mouseWheelCallback(wheel,direction, x, y);
}

VRFreeGLUTInputDevice::VRFreeGLUTInputDevice() {
	device = this;
}

VRFreeGLUTInputDevice::~VRFreeGLUTInputDevice() {
}

void VRFreeGLUTInputDevice::appendNewInputEventsSinceLastCall(VRDataQueue* queue) {
	for (std::vector<int>::iterator it = _windows.begin(); it != _windows.end(); ++it){
			glutSetWindow(*it);
			glutMainLoopEvent();
	}


    for (size_t f = 0; f < _events.size(); f++)
    {
    	queue->push(_events[f]);
    }
    _events.clear();
}

void VRFreeGLUTInputDevice::addWindow(int window) {
	glutKeyboardFunc(freeglut_key_callback);
	glutKeyboardUpFunc(freeglut_key_callback_up);
	glutSpecialFunc(freeglut_specialkey_callback);
	glutSpecialUpFunc(freeglut_specialkey_callback_up);
	glutMotionFunc(freeglut_mousemove_callback);
	glutPassiveMotionFunc(freeglut_mousemove_passive_callback);
	glutMouseFunc(freeglut_mouse_callback);
    glutMouseWheelFunc(freeglut_mousewheel_callback);
    _windows.push_back(window);
}

void VRFreeGLUTInputDevice::keyCallback(int key, int x, int y, bool isDown, bool isSpecial) {
    std::string actionName = isDown? "Down" : "Up";
    std::string name = "Kbd" + getKeyName(key,isSpecial) + "_" + actionName;

    VRDataIndex event = VRButtonEvent::createValidDataIndex(name, (int)isDown);
    _events.push_back(event);
}


void VRFreeGLUTInputDevice::cursorPositionCallback(int xpos, int ypos, bool isActive) {

    std::vector<float> pos;
    pos.push_back((float)xpos);
    pos.push_back((float)ypos);

    std::vector<float> npos = pos;
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    npos[0] /= (float)width;
    npos[1] /= (float)height;

    VRDataIndex event = VRCursorEvent::createValidDataIndex("Mouse_Move", pos, npos);
    _events.push_back(event);
}


void VRFreeGLUTInputDevice::mouseWheelCallback(int wheel, int direction, int x, int y){
    VRDataIndex event = VRAnalogEvent::createValidDataIndex("MouseWheel_Spin", (float)(direction*wheel));
    event.addData("Wheel", wheel);
    event.addData("Direction", direction);
    event.addData("Turns", direction*wheel);
    _events.push_back(event);
}

void VRFreeGLUTInputDevice::mouseButtonCallback(int button, int state, int x, int y) {

    std::string buttonStr;
    if (button == GLUT_LEFT_BUTTON) {
        buttonStr = "MouseBtnLeft_";
    }
    else if (button == GLUT_RIGHT_BUTTON) {
        buttonStr = "MouseBtnRight_";
    }
    else if (button == GLUT_MIDDLE_BUTTON) {
        buttonStr = "MouseBtnMiddle_";
    }
    else {
        std::ostringstream os;
        os << button;
        buttonStr = "MouseBtn" + os.str() + "_";
    }

    std::string actionStr;
    if (state == GLUT_DOWN) {
        actionStr = "Down";
    }
    else if (state == GLUT_UP) {
        actionStr = "Up";
    }

    std::string name = buttonStr + actionStr;
    VRDataIndex event = VRButtonEvent::createValidDataIndex(name, state);
    _events.push_back(event);
}


std::string getKeyName(int key, bool isSpecial) {
	if(!isSpecial){
		switch ((unsigned char) key)
		{
			case 27:
				return "Esc";

			default:
				stringstream s;
				s << (unsigned char) key;
				return s.str();
		}
    }else{
		switch (key)
		{
			case GLUT_KEY_F1:           return "F1";
			case GLUT_KEY_F2:           return "F2";
			case GLUT_KEY_F3:           return "F3";
			case GLUT_KEY_F4:           return "F4";
			case GLUT_KEY_F5:           return "F5";
			case GLUT_KEY_F6:           return "F6";
			case GLUT_KEY_F7:           return "F7";
			case GLUT_KEY_F8:           return "F8";
			case GLUT_KEY_F9:           return "F9";
			case GLUT_KEY_F10:          return "F10";
			case GLUT_KEY_F11:          return "F11";
			case GLUT_KEY_F12:          return "F12";
			case GLUT_KEY_PAGE_UP:      return "PageUp";
			case GLUT_KEY_PAGE_DOWN:    return "PageDown";
			case GLUT_KEY_HOME:         return "Home";
			case GLUT_KEY_END:          return "End";
			case GLUT_KEY_UP:           return "Up";
			case GLUT_KEY_DOWN:         return "Down";
			case GLUT_KEY_LEFT:         return "Left";
			case GLUT_KEY_RIGHT:        return "Right";
			case GLUT_KEY_INSERT:       return "Insert";
		}
	}
	return "";
}

} /* namespace MinVR */

