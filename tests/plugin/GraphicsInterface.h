/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef GRAPHICSINTERFACE_H_
#define GRAPHICSINTERFACE_H_

#include "plugin/PluginInterface.h"

class GraphicsDriver
{
public:
	virtual ~GraphicsDriver() {}
	virtual void draw() = 0;
};

class GraphicsInterface : public MinVR::PluginInterface {
public:
	virtual void addGraphicsDriver(std::string name, GraphicsDriver* driver) = 0;

	std::string getName() { return "GraphicsInterface"; }
};

#endif /* GRAPHICSINTERFACE_H_ */

