/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <display/concrete/CommandLineDisplay.h>
#include <iostream>

using namespace std;

namespace MinVR {

CommandLineDisplay::CommandLineDisplay() {
	cout << "Created command line display" << endl;
}

CommandLineDisplay::~CommandLineDisplay() {
	cout << "Destroyed command line display" << endl;
}

void CommandLineDisplay::finishRendering() {
	cout << "Finish rendering" << endl;
	finishRenderingAllDisplays();
}

void CommandLineDisplay::startRendering(const VRRenderer& renderer,
		VRRenderState& state) {
	cout << "Start rendering" << endl;
	startRenderingAllDisplays(renderer, state);
}

void CommandLineDisplay::useDisplay(const MinVR::VRDisplayAction& action) {
	cout << "Using display" << endl;
    if (getSubDisplays().size() > 0)
    {
        for (int f = 0; f < getSubDisplays().size(); f++)
        {
            getSubDisplays()[f]->use(action);
        }
    }
    else
    {
        // Do action for non graphics only
    }
	cout << "Finish using display" << endl;
}

CommandLineDisplayFactory::CommandLineDisplayFactory() {
}

CommandLineDisplayFactory::~CommandLineDisplayFactory() {
}

VRDisplayDevice* CommandLineDisplayFactory::createDisplay(const std::string type, const std::string name, VRDataIndex& config, VRDisplayDeviceFactory* factory){
	if (type == "commandline")
	{
		return new CommandLineDisplay();
	}

	return NULL;
}


} /* namespace MinVR */

