/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef TESTPLUGINDRIVERS_H_
#define TESTPLUGINDRIVERS_H_

class OpenGLGraphicsDriver : public GraphicsDriver
{
public:
	PLUGIN_API void draw() { std::cout << "Render opengl" << std::endl; }
};

class D3DGraphicsDriver : public GraphicsDriver
{
public:
	PLUGIN_API void draw() { std::cout << "Render D3D" << std::endl; }
};

class VRPNFactory : public InputDeviceFactory
{
public:
	PLUGIN_API bool createDevice(std::string type, std::string parameters)
	{
		if (type == "VRPNButton")
		{
			std::cout << "Created VRPNButton" << std::endl;
			return true;
		}
		else if (type == "VRPNTracker")
		{
			std::cout << "Created VRPNTracker" << std::endl;
			return true;
		}

		return false;
	}
};

class TouchFactory : public InputDeviceFactory
{
public:
	PLUGIN_API bool createDevice(std::string type, std::string parameters)
	{
		if (type == "TUIO")
		{
			std::cout << "Created TUIO device" << std::endl;
			return true;
		}
		if (type == "Touch")
		{
			std::cout << "Created Touch device" << std::endl;
			return true;
		}

		return false;
	}
};

#endif /* TESTPLUGINDRIVERS_H_ */
