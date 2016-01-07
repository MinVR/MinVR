/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <GlfwInputDevice.h>
#include <ctime>
#include <cctype>

namespace MinVR {

std::string getGlfwKeyName(int key);
std::string getGlfwKeyValue(int key, int mods);
std::string getGlfwActionName(int action);

GlfwInputDevice::GlfwInputDevice() {
}

GlfwInputDevice::~GlfwInputDevice() {
}

void GlfwInputDevice::appendNewInputEventsSinceLastCall(VRDataQueue& queue) {
    glfwPollEvents();
    for (int f = 0; f < events.size(); f++)
    {
    	queue.push(events[f]);
    }

    events.clear();
}

void GlfwInputDevice::keyCallback(GLFWwindow* window, int key, int scancode,
		int action, int mods) {

	std::string event = getGlfwKeyName(key) + "_" + getGlfwActionName(action);
	dataIndex.addData("/keyboard/value", event);
	dataIndex.addData("/keyboard/timestamp", (int)clock());
	events.push_back(dataIndex.serialize("/keyboard"));
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode,
		int action, int mods) {
	((GlfwInputDevice*)(glfwGetWindowUserPointer(window)))->keyCallback(window, key, scancode, action, mods);
}


std::vector<VRInputDevice*> GlfwInputDeviceFactory::create(
		const VRDataIndex& dataIndex) {
	std::vector<VRInputDevice*> devices;
	devices.push_back(device);
	return devices;
}

void GlfwInputDevice::registerGlfwWindow(GLFWwindow* window) {
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, glfw_key_callback);
}

std::string getGlfwKeyName(int key)
{
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
        case GLFW_KEY_SPACE:        return "SPACE";
        case GLFW_KEY_MINUS:        return "MINUS";
        case GLFW_KEY_EQUAL:        return "EQUAL";
        case GLFW_KEY_LEFT_BRACKET: return "LEFT_BRACKET";
        case GLFW_KEY_RIGHT_BRACKET: return "RIGHT_BRACKET";
        case GLFW_KEY_BACKSLASH:    return "BACKSLASH";
        case GLFW_KEY_SEMICOLON:    return "SEMICOLON";
        case GLFW_KEY_APOSTROPHE:   return "APOSTROPHE";
        case GLFW_KEY_GRAVE_ACCENT: return "GRAVE_ACCENT";
        case GLFW_KEY_COMMA:        return "COMMA";
        case GLFW_KEY_PERIOD:       return "PERIOD";
        case GLFW_KEY_SLASH:        return "SLASH";
        case GLFW_KEY_WORLD_1:      return "WORLD1";
        case GLFW_KEY_WORLD_2:      return "WORLD2";

        // Function keys
        case GLFW_KEY_ESCAPE:       return "ESC";
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
        case GLFW_KEY_UP:           return "UP";
        case GLFW_KEY_DOWN:         return "DOWN";
        case GLFW_KEY_LEFT:         return "LEFT";
        case GLFW_KEY_RIGHT:        return "RIGHT";
        case GLFW_KEY_LEFT_SHIFT:   return "LEFT_SHIFT";
        case GLFW_KEY_RIGHT_SHIFT:  return "RIGHT_SHIFT";
        case GLFW_KEY_LEFT_CONTROL: return "LEFT_CONTROL";
        case GLFW_KEY_RIGHT_CONTROL: return "RIGHT_CONTROL";
        case GLFW_KEY_LEFT_ALT:     return "LEFT_ALT";
        case GLFW_KEY_RIGHT_ALT:    return "RIGHT_ALT";
        case GLFW_KEY_TAB:          return "TAB";
        case GLFW_KEY_ENTER:        return "ENTER";
        case GLFW_KEY_BACKSPACE:    return "BACKSPACE";
        case GLFW_KEY_INSERT:       return "INSERT";
        case GLFW_KEY_DELETE:       return "DELETE";
        case GLFW_KEY_PAGE_UP:      return "PAGEUP";
        case GLFW_KEY_PAGE_DOWN:    return "PAGEDOWN";
        case GLFW_KEY_HOME:         return "HOME";
        case GLFW_KEY_END:          return "END";
        case GLFW_KEY_KP_0:         return "KEYPAD0";
        case GLFW_KEY_KP_1:         return "KEYPAD1";
        case GLFW_KEY_KP_2:         return "KEYPAD2";
        case GLFW_KEY_KP_3:         return "KEYPAD3";
        case GLFW_KEY_KP_4:         return "KEYPAD4";
        case GLFW_KEY_KP_5:         return "KEYPAD5";
        case GLFW_KEY_KP_6:         return "KEYPAD6";
        case GLFW_KEY_KP_7:         return "KEYPAD7";
        case GLFW_KEY_KP_8:         return "KEYPAD8";
        case GLFW_KEY_KP_9:         return "KEYPAD9";
        case GLFW_KEY_KP_DIVIDE:    return "KEYPAD_DIVIDE";
        case GLFW_KEY_KP_MULTIPLY:  return "KEYPAD_MULTIPLY";
        case GLFW_KEY_KP_SUBTRACT:  return "KEYPAD_SUBTRACT";
        case GLFW_KEY_KP_ADD:       return "KEYPAD_ADD";
        case GLFW_KEY_KP_DECIMAL:   return "KEYPAD_DECIMAL";
        case GLFW_KEY_KP_EQUAL:     return "KEYPAD_EQUAL";
        case GLFW_KEY_KP_ENTER:     return "KEYPAD_ENTER";
        case GLFW_KEY_PRINT_SCREEN: return "PRINT_SCREEN";
        case GLFW_KEY_NUM_LOCK:     return "NUM_LOCK";
        case GLFW_KEY_CAPS_LOCK:    return "CAPS_LOCK";
        case GLFW_KEY_SCROLL_LOCK:  return "SCROLL_LOCK";
        case GLFW_KEY_PAUSE:        return "PAUSE";
        case GLFW_KEY_LEFT_SUPER:   return "LEFT_SUPER";
        case GLFW_KEY_RIGHT_SUPER:  return "RIGHT_SUPER";
        case GLFW_KEY_MENU:         return "MENU";
        case GLFW_KEY_UNKNOWN:      return "UNKNOWN";

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
            return "down";
        case GLFW_RELEASE:
            return "up";
        case GLFW_REPEAT:
            return "repeat";
    }

    return "caused unknown action";
}


} /* namespace MinVR */

