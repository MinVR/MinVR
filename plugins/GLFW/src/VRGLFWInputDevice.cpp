/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "VRGLFWInputDevice.h"
#include <ctime>
#include <cctype>
#include <api/VRButtonEvent.h>
#include <api/VRCursorEvent.h>

namespace MinVR {

std::string getGlfwKeyName(int key);
std::string getGlfwKeyValue(int key, int mods);
std::string getGlfwActionName(int action);

static void glfw_key_callback(GLFWwindow* window, int key, int scancode,
        int action, int mods) {
    ((VRGLFWInputDevice*)(glfwGetWindowUserPointer(window)))->keyCallback(window, key, scancode, action, mods);
}


static void glfw_size_callback(GLFWwindow* window, int width, int height) {
    ((VRGLFWInputDevice*)(glfwGetWindowUserPointer(window)))->sizeCallback(window, width, height);
}


static void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    ((VRGLFWInputDevice*)(glfwGetWindowUserPointer(window)))->cursorPositionCallback(window, (float)xpos, (float)ypos);
}


static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  ((VRGLFWInputDevice*)(glfwGetWindowUserPointer(window)))->mouseButtonCallback(window, button, action, mods);
}


VRGLFWInputDevice::VRGLFWInputDevice() {
}

VRGLFWInputDevice::~VRGLFWInputDevice() {
}

void VRGLFWInputDevice::appendNewInputEventsSinceLastCall(VRDataQueue* queue) {
    glfwPollEvents();

    for (size_t f = 0; f < _events.size(); f++)
    {
    	queue->push(_events[f].serialize());
    }

    _events.clear();
}

void VRGLFWInputDevice::addWindow(GLFWwindow* window) {
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetWindowSizeCallback(window, glfw_size_callback);
    glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(window, glfw_cursor_position_callback);
    _windows.push_back(window);
}

void VRGLFWInputDevice::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    std::string actionName = getGlfwActionName(action);
    std::string name = "Kbd" + getGlfwKeyName(key) + "_" + actionName;

    VRDataIndex event = VRButtonEvent::createValidDataIndex(name, action);
    _events.push_back(event);
}

void VRGLFWInputDevice::sizeCallback(GLFWwindow* window, int width, int height) {
    // TODO: create an event reporting to MinVR that the size has changed
}


void VRGLFWInputDevice::cursorPositionCallback(GLFWwindow* window, float xpos, float ypos) {
    std::vector<float> pos;
    pos.push_back(xpos);
    pos.push_back(ypos);
    
    std::vector<float> npos = pos;
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    npos[0] /= (float)width;
    npos[1] /= (float)height;
    
    VRDataIndex event = VRCursorEvent::createValidDataIndex("Mouse_Move", pos, npos);
    _events.push_back(event);
}



void VRGLFWInputDevice::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    std::string buttonStr;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        buttonStr = "MouseBtnLeft_";
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        buttonStr = "MouseBtnRight_";
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        buttonStr = "MouseBtnMiddle_";
    }
    else {
        std::ostringstream os;
        os << button;
        buttonStr = "MouseBtn" + os.str() + "_";
    }

    std::string actionStr;
    if (action == GLFW_PRESS) {
        actionStr = "Down";
    }
    else if (action == GLFW_RELEASE) {
        actionStr = "Up";
    }

    std::string name = buttonStr + actionStr;
    VRDataIndex event = VRButtonEvent::createValidDataIndex(name, action);
    _events.push_back(event);
}





std::string getGlfwKeyName(int key) {
    switch (key)
    {
        // Printable keys
        case GLFW_KEY_A:            return "A";
        case GLFW_KEY_B:            return "B";
        case GLFW_KEY_C:            return "C";
        case GLFW_KEY_D:            return "D";
        case GLFW_KEY_E:            return "E";
        case GLFW_KEY_F:            return "F";
        case GLFW_KEY_G:            return "G";
        case GLFW_KEY_H:            return "H";
        case GLFW_KEY_I:            return "I";
        case GLFW_KEY_J:            return "J";
        case GLFW_KEY_K:            return "K";
        case GLFW_KEY_L:            return "L";
        case GLFW_KEY_M:            return "M";
        case GLFW_KEY_N:            return "N";
        case GLFW_KEY_O:            return "O";
        case GLFW_KEY_P:            return "P";
        case GLFW_KEY_Q:            return "Q";
        case GLFW_KEY_R:            return "R";
        case GLFW_KEY_S:            return "S";
        case GLFW_KEY_T:            return "T";
        case GLFW_KEY_U:            return "U";
        case GLFW_KEY_V:            return "V";
        case GLFW_KEY_W:            return "W";
        case GLFW_KEY_X:            return "X";
        case GLFW_KEY_Y:            return "Y";
        case GLFW_KEY_Z:            return "Z";
        case GLFW_KEY_1:            return "1";
        case GLFW_KEY_2:            return "2";
        case GLFW_KEY_3:            return "3";
        case GLFW_KEY_4:            return "4";
        case GLFW_KEY_5:            return "5";
        case GLFW_KEY_6:            return "6";
        case GLFW_KEY_7:            return "7";
        case GLFW_KEY_8:            return "8";
        case GLFW_KEY_9:            return "9";
        case GLFW_KEY_0:            return "0";
        case GLFW_KEY_SPACE:        return "Space";
        case GLFW_KEY_MINUS:        return "Minus";
        case GLFW_KEY_EQUAL:        return "Equal";
        case GLFW_KEY_LEFT_BRACKET: return "LeftBracket";
        case GLFW_KEY_RIGHT_BRACKET: return "RightBracket";
        case GLFW_KEY_BACKSLASH:    return "Backslash";
        case GLFW_KEY_SEMICOLON:    return "Semicolon";
        case GLFW_KEY_APOSTROPHE:   return "Apostrophe";
        case GLFW_KEY_GRAVE_ACCENT: return "GraveAccent";
        case GLFW_KEY_COMMA:        return "Comma";
        case GLFW_KEY_PERIOD:       return "Period";
        case GLFW_KEY_SLASH:        return "Slash";
        case GLFW_KEY_WORLD_1:      return "World1";
        case GLFW_KEY_WORLD_2:      return "World2";

        // Function keys
        case GLFW_KEY_ESCAPE:       return "Esc";
        case GLFW_KEY_F1:           return "F1";
        case GLFW_KEY_F2:           return "F2";
        case GLFW_KEY_F3:           return "F3";
        case GLFW_KEY_F4:           return "F4";
        case GLFW_KEY_F5:           return "F5";
        case GLFW_KEY_F6:           return "F6";
        case GLFW_KEY_F7:           return "F7";
        case GLFW_KEY_F8:           return "F8";
        case GLFW_KEY_F9:           return "F9";
        case GLFW_KEY_F10:          return "F10";
        case GLFW_KEY_F11:          return "F11";
        case GLFW_KEY_F12:          return "F12";
        case GLFW_KEY_F13:          return "F13";
        case GLFW_KEY_F14:          return "F14";
        case GLFW_KEY_F15:          return "F15";
        case GLFW_KEY_F16:          return "F16";
        case GLFW_KEY_F17:          return "F17";
        case GLFW_KEY_F18:          return "F18";
        case GLFW_KEY_F19:          return "F19";
        case GLFW_KEY_F20:          return "F20";
        case GLFW_KEY_F21:          return "F21";
        case GLFW_KEY_F22:          return "F22";
        case GLFW_KEY_F23:          return "F23";
        case GLFW_KEY_F24:          return "F24";
        case GLFW_KEY_F25:          return "F25";
        case GLFW_KEY_UP:           return "Up";
        case GLFW_KEY_DOWN:         return "Down";
        case GLFW_KEY_LEFT:         return "Left";
        case GLFW_KEY_RIGHT:        return "Right";
        case GLFW_KEY_LEFT_SHIFT:   return "LeftShift";
        case GLFW_KEY_RIGHT_SHIFT:  return "RightShift";
        case GLFW_KEY_LEFT_CONTROL: return "LeftControl";
        case GLFW_KEY_RIGHT_CONTROL: return "RightControl";
        case GLFW_KEY_LEFT_ALT:     return "LeftAlt";
        case GLFW_KEY_RIGHT_ALT:    return "RightAlt";
        case GLFW_KEY_TAB:          return "Tab";
        case GLFW_KEY_ENTER:        return "Enter";
        case GLFW_KEY_BACKSPACE:    return "Backspace";
        case GLFW_KEY_INSERT:       return "Insert";
        case GLFW_KEY_DELETE:       return "Delete";
        case GLFW_KEY_PAGE_UP:      return "PageUp";
        case GLFW_KEY_PAGE_DOWN:    return "PageDown";
        case GLFW_KEY_HOME:         return "Home";
        case GLFW_KEY_END:          return "End";
        case GLFW_KEY_KP_0:         return "Keypad0";
        case GLFW_KEY_KP_1:         return "Keypad1";
        case GLFW_KEY_KP_2:         return "Keypad2";
        case GLFW_KEY_KP_3:         return "Keypad3";
        case GLFW_KEY_KP_4:         return "Keypad4";
        case GLFW_KEY_KP_5:         return "Keypad5";
        case GLFW_KEY_KP_6:         return "Keypad6";
        case GLFW_KEY_KP_7:         return "Keypad7";
        case GLFW_KEY_KP_8:         return "Keypad8";
        case GLFW_KEY_KP_9:         return "Keypad9";
        case GLFW_KEY_KP_DIVIDE:    return "KeypadDivide";
        case GLFW_KEY_KP_MULTIPLY:  return "KeypadMultiply";
        case GLFW_KEY_KP_SUBTRACT:  return "KeypadSubtract";
        case GLFW_KEY_KP_ADD:       return "KeypadAdd";
        case GLFW_KEY_KP_DECIMAL:   return "KeypadDecimal";
        case GLFW_KEY_KP_EQUAL:     return "KeypadEqual";
        case GLFW_KEY_KP_ENTER:     return "KeypadEnter";
        case GLFW_KEY_PRINT_SCREEN: return "PrintScreen";
        case GLFW_KEY_NUM_LOCK:     return "NumLock";
        case GLFW_KEY_CAPS_LOCK:    return "CapsLock";
        case GLFW_KEY_SCROLL_LOCK:  return "ScrollLock";
        case GLFW_KEY_PAUSE:        return "Pause";
        case GLFW_KEY_LEFT_SUPER:   return "LeftSuper";
        case GLFW_KEY_RIGHT_SUPER:  return "RightSuper";
        case GLFW_KEY_MENU:         return "Menu";
        case GLFW_KEY_UNKNOWN:      return "Unknown";

        default:                    return "";
    }
}

std::string getGlfwKeyValue(int key, int mods)
{
	std::string value = "";
	switch (key)
    {
        // Printable keys
        case GLFW_KEY_A:            value = "a"; break;
        case GLFW_KEY_B:            value = "b"; break;
        case GLFW_KEY_C:            value = "c"; break;
        case GLFW_KEY_D:            value = "d"; break;
        case GLFW_KEY_E:            value = "e"; break;
        case GLFW_KEY_F:            value = "f"; break;
        case GLFW_KEY_G:            value = "g"; break;
        case GLFW_KEY_H:            value = "h"; break;
        case GLFW_KEY_I:            value = "i"; break;
        case GLFW_KEY_J:            value = "j"; break;
        case GLFW_KEY_K:            value = "k"; break;
        case GLFW_KEY_L:            value = "l"; break;
        case GLFW_KEY_M:            value = "m"; break;
        case GLFW_KEY_N:            value = "n"; break;
        case GLFW_KEY_O:            value = "o"; break;
        case GLFW_KEY_P:            value = "p"; break;
        case GLFW_KEY_Q:            value = "q"; break;
        case GLFW_KEY_R:            value = "r"; break;
        case GLFW_KEY_S:            value = "s"; break;
        case GLFW_KEY_T:            value = "t"; break;
        case GLFW_KEY_U:            value = "u"; break;
        case GLFW_KEY_V:            value = "v"; break;
        case GLFW_KEY_W:            value = "w"; break;
        case GLFW_KEY_X:            value = "x"; break;
        case GLFW_KEY_Y:            value = "y"; break;
        case GLFW_KEY_Z:            value = "z"; break;

		default:					value = ""; break;
	}

	if (value != "") {
		if (mods & GLFW_MOD_SHIFT)
		{
			value = std::toupper(value[0]);
		}
	}
	else {

		switch(key)
		{
			case GLFW_KEY_1:            value = "1"; break;
			case GLFW_KEY_2:            value = "2"; break;
			case GLFW_KEY_3:            value = "3"; break;
			case GLFW_KEY_4:            value = "4"; break;
			case GLFW_KEY_5:            value = "5"; break;
			case GLFW_KEY_6:            value = "6"; break;
			case GLFW_KEY_7:            value = "7"; break;
			case GLFW_KEY_8:            value =  "8"; break;
			case GLFW_KEY_9:            value = "9"; break;
			case GLFW_KEY_0:            value = "0"; break;
			case GLFW_KEY_SPACE:        value = " "; break;
			case GLFW_KEY_MINUS:        value = "-"; break;
			case GLFW_KEY_EQUAL:        value = "="; break;
			case GLFW_KEY_LEFT_BRACKET: value = "]"; break;
			case GLFW_KEY_RIGHT_BRACKET: value = "["; break;
			case GLFW_KEY_BACKSLASH:    value = "\\"; break;
			case GLFW_KEY_SEMICOLON:    value = ";"; break;
			case GLFW_KEY_APOSTROPHE:   value = "'"; break;
			case GLFW_KEY_GRAVE_ACCENT: value = "`"; break;
			case GLFW_KEY_COMMA:        value = ","; break;
			case GLFW_KEY_PERIOD:       value = "."; break;
			case GLFW_KEY_SLASH:        value = "/"; break;
			case GLFW_KEY_TAB:          value = "\t"; break;
			case GLFW_KEY_ENTER:        value = "\n"; break;
			case GLFW_KEY_KP_0:         value = "0"; break;
			case GLFW_KEY_KP_1:         value = "1"; break;
			case GLFW_KEY_KP_2:         value = "2"; break;
			case GLFW_KEY_KP_3:         value = "3"; break;
			case GLFW_KEY_KP_4:         value = "4"; break;
			case GLFW_KEY_KP_5:         value = "5"; break;
			case GLFW_KEY_KP_6:         value = "6"; break;
			case GLFW_KEY_KP_7:         value = "7"; break;
			case GLFW_KEY_KP_8:         value = "8"; break;
			case GLFW_KEY_KP_9:         value = "9"; break;
			case GLFW_KEY_KP_DIVIDE:    value = "/"; break;
			case GLFW_KEY_KP_MULTIPLY:  value = "*"; break;
			case GLFW_KEY_KP_SUBTRACT:  value = "-"; break;
			case GLFW_KEY_KP_ADD:       value = "+"; break;
			case GLFW_KEY_KP_DECIMAL:   value = "."; break;
			case GLFW_KEY_KP_EQUAL:     value = "="; break;
			case GLFW_KEY_KP_ENTER:     value = "\n"; break;

			default:                    break;
		}
	}

	return value;
}

std::string getGlfwActionName(int action)
{
    switch (action)
    {
        case GLFW_PRESS:
            return "Down";
        case GLFW_RELEASE:
            return "Up";
        case GLFW_REPEAT:
            return "Repeat";
    }

    return "caused unknown action";
}



} /* namespace MinVR */

